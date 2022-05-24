/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FeedbackAudioProcessor::FeedbackAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true)  // mono() vs. stero()
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::mono(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters()),
                          forwardFFT(fftOrder),
                          window(fftSize, juce::dsp::WindowingFunction<float>::hann)
                            
#endif
{
}

FeedbackAudioProcessor::~FeedbackAudioProcessor()
{
}

//==============================================================================
const juce::String FeedbackAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FeedbackAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FeedbackAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FeedbackAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FeedbackAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FeedbackAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FeedbackAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FeedbackAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FeedbackAudioProcessor::getProgramName (int index)
{
    return {};
}

void FeedbackAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FeedbackAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // set initial values for waveTable
    curSampleRate = sampleRate;
    phase = 0;
    wtSize = fftSize;
    for (int i = 0; i < wtSize; i++)
    {
        waveTable.insert(i, sin(2.0 * juce::double_Pi * i / wtSize));
    }

    // feedback gain & frequency interpolation for avoiding pops and clicks and smoothness
    feedbackRamp.reset(curSampleRate, 0.005);
    frequencyRamp.reset(curSampleRate, 0.025);
}

void FeedbackAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FeedbackAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void FeedbackAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (totalNumInputChannels > 0)
    {
        // get data for mono and feedbackGain (with interpolation)
        auto* channelData = buffer.getWritePointer(0);
        feedbackGain = apvts.getRawParameterValue("FEEDBACK")->load();
        auto offsetValue = apvts.getRawParameterValue("OFFSET")->load();
        auto detuneValue = apvts.getRawParameterValue("DETUNE")->load();

        feedbackRamp.setTargetValue(feedbackGain);

        for (int sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            pushNextSampleIntoFifo(channelData[sample]);
            
            previousFrequency = frequency;
            auto tempFrequency = frequencyRamp.getNextValue() * std::pow(semitoneConstant, offsetValue) + detuneValue;
            increment = tempFrequency * wtSize / curSampleRate;
            channelData[sample] += waveTable[(int)phase] * (feedbackRamp.getNextValue() / 2);
            phase = fmod((phase + increment), wtSize);
        }
    }

    // Gain ramp for main "gain out"
    gain = apvts.getRawParameterValue("GAIN")->load();
    if (gain == previousGain) {
        buffer.applyGain(gain);
    }
    else {
        buffer.applyGainRamp(0, buffer.getNumSamples(), previousGain, gain);
        previousGain = gain;
    }
}

// Helper function for pushSample: finds fundamental and decides if infinite sustain
void FeedbackAudioProcessor::isSustain()
{
    frequency = getFundamentalFrequency();
    if (frequency < lowestGuitarFreq || frequency > highestGuitarFreq)
    {
        frequency = previousFrequency;
    }
    else {
        // if not changes set the target 
        frequencyRamp.setTargetValue(frequency);
    }
}

// helper function for process block - pushes sample into the fifo array 
// calls other helpers to set correct frequency
void FeedbackAudioProcessor::pushNextSampleIntoFifo(float sample) noexcept
{
    // condition for FFT being ready (full)
    if (fifoIndex == fftSize)      
    {
        std::fill(fftData.begin(), fftData.end(), 0.0f);
        std::copy(fifo.begin(), fifo.end(), fftData.begin()); 
        fifoIndex = 0;
        window.multiplyWithWindowingTable(fftData.data(), fftSize);
        forwardFFT.performFrequencyOnlyForwardTransform(fftData.data());
        isSustain();
    }
    fifo[(size_t)fifoIndex++] = sample; 
}

float FeedbackAudioProcessor::getFundamentalFrequency()
{
    // tolerance determines how easy it is to generate feedback
    auto toleranceValue = apvts.getRawParameterValue("TOLERANCE")->load();
    float max = 0.f;
    int index = 0;
    float absVal;

    for (auto i = 0; i < fftSize; i++)
    {
        absVal = abs(fftData[i]);
        if (absVal > max)
        {
            max = absVal;
            index = i;
        }
    }
    if (max > (toleranceValue * toleranceConstant))
    {
        return (float)index / (fftSize - 1) * curSampleRate;
    }
    return 0;
}

//==============================================================================
bool FeedbackAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FeedbackAudioProcessor::createEditor()
{
    return new FeedbackAudioProcessorEditor (*this);
}

//==============================================================================
void FeedbackAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FeedbackAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FeedbackAudioProcessor();
}

// Initializes the audioParameters and creates a vector of them for the APVTS  
juce::AudioProcessorValueTreeState::ParameterLayout FeedbackAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    juce::NormalisableRange<float> rangeFeedback = juce::NormalisableRange<float>(0.0f, 1.0f, 0.0001f, 1.0, false);

    params.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN", "Gain", 0.0f, 1.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("FEEDBACK", "Feedback", rangeFeedback, 0.0f, juce::String(), 
                                                                    juce::AudioProcessorParameter::genericParameter, nullptr, nullptr));
    params.push_back(std::make_unique<juce::AudioParameterInt>("OFFSET", "Offset", 0, 24, 12));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("TOLERANCE", "Tolerance", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DETUNE", "Detune", -50.0f, 50.0f, 0.0f));

    return { params.begin(), params.end() };
}