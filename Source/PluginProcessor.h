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

// In my opinion it's better to use a namespace for parameter IDs just so you don't risk misspelling. You could put this in a separate file if you think it'd be cleaner
namespace ParamIDs
{
    inline constexpr auto Gain { "Gain" };
    inline constexpr auto Feedback { "Feedback" };
    inline constexpr auto Offset { "Offset" };
    inline constexpr auto Tolerance { "Tolerance" };
    inline constexpr auto Detune { "Detune" };
};

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
    juce::LinearSmoothedValue<float> feedbackRamp { 0.0f };
    juce::LinearSmoothedValue<float> frequencyRamp { 0.0f };
    //juce::LinearSmoothedValue<float> detuneRamp{ 0.0f };

    // PRIVATE MEMBER VARIABLES FOR FFT
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;
    std::array<float, fftSize> fifo;
    std::array<float, fftSize * 2> fftData;
    int fifoIndex = 0;

    double curSampleRate;
    // none of your code actually utilized the frequency and previousFrequency values in a meaningful way so I deleted them forever :)

    float gain;
    float previousGain;

    // using a lookup table lets you do linear interpolation between values so you should get a slightly cleaner sine signal
    juce::dsp::LookupTable<float> waveTable;
    double wtSize;
    double phase;
//    float feedbackGain; IMO you don't need this

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FeedbackAudioProcessor)
};
