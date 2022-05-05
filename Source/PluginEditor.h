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
    //void sliderValueChanged(juce::Slider* slider) override;

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

    // Att.
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackGainSliderAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> offsetSliderAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toleranceSliderAtt;

    FeedbackAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FeedbackAudioProcessorEditor)
};
