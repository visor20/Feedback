/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class FeedbackAudioProcessorEditor  : public juce::AudioProcessorEditor                             
{
public:
    FeedbackAudioProcessorEditor (FeedbackAudioProcessor&);
    ~FeedbackAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    // sliders and labels 
    juce::Slider mGainSlider;
    juce::Label mGainLabel;
    juce::Slider mFeedbackGainSlider;
    juce::Label mFeedbackGainLabel;
    juce::Slider mOffsetSlider;
    juce::Label mOffsetLabel;
    juce::Slider mToleranceSlider;
    juce::Label mToleranceLabel;
    juce::Slider mDetuneSlider;
    juce::Label mDetuneLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackGainSliderAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> offsetSliderAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toleranceSliderAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> detuneSliderAtt;

    FeedbackAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& apvts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FeedbackAudioProcessorEditor)
};
