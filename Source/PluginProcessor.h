/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class FeedbackAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    FeedbackAudioProcessor();
    ~FeedbackAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;


    void pushNextSampleIntoFifo(float sample) noexcept;
    float getFundamentalFrequency();
    void isSustain();

    // value tree for parameters 
    juce::AudioProcessorValueTreeState apvts;

    // constants 
    static constexpr auto fftOrder = 12;          /* 12 creates enough detail without huge lag - 10 is safest */
    static constexpr auto fftSize = 1 << fftOrder;
    static constexpr auto lowestGuitarFreq = 75;
    static constexpr auto highestGuitarFreq = 1200;
    static constexpr auto semitoneConstant = 1.05945;
    static constexpr auto toleranceConstant = 250;

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    // ramp for feedback gain 
    juce::LinearSmoothedValue<float> feedbackRamp{ 0.0f };
    juce::LinearSmoothedValue<float> frequencyRamp{ 0.0f };
    //juce::LinearSmoothedValue<float> detuneRamp{ 0.0f };

    // PRIVATE MEMBER VARIABLES FOR FFT
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;
    std::array<float, fftSize> fifo;
    std::array<float, fftSize * 2> fftData;
    int fifoIndex = 0;

    double curSampleRate;
    float frequency;
    float previousFrequency = 0;

    float gain;
    float previousGain;

    juce::Array<float> waveTable;
    double wtSize;
    double phase;
    double increment;
    float feedbackGain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FeedbackAudioProcessor)
};
