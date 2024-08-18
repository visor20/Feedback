/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FeedbackAudioProcessorEditor::FeedbackAudioProcessorEditor (FeedbackAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), apvts (audioProcessor.apvts)
{
    setSize(600, 300);

    // Gain Slider 
    mGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    mGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(mGainSlider);
    gainSliderAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, ParamIDs::Gain, mGainSlider);

    // Gain Label 
    addAndMakeVisible(mGainLabel);
    mGainLabel.setText("Gain Out", juce::dontSendNotification);
    mGainLabel.attachToComponent(&mGainSlider, false);
    mGainLabel.setJustificationType(juce::Justification::centred);

    // Feedback Gain Slider 
    mFeedbackGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    mFeedbackGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(mFeedbackGainSlider);
    feedbackGainSliderAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, ParamIDs::Feedback, mFeedbackGainSlider);

    // Feedback Gain Label
    addAndMakeVisible(mFeedbackGainLabel);
    mFeedbackGainLabel.setText("Feedback Gain", juce::dontSendNotification);
    mFeedbackGainLabel.attachToComponent(&mFeedbackGainSlider, false);
    mFeedbackGainLabel.setJustificationType(juce::Justification::centred);

    // Offset Slider 
    mOffsetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mOffsetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(mOffsetSlider);
    offsetSliderAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, ParamIDs::Offset, mOffsetSlider);

    // Offset Label 
    addAndMakeVisible(mOffsetLabel);
    mOffsetLabel.setText("Feedback Offset", juce::dontSendNotification);
    mOffsetLabel.attachToComponent(&mOffsetSlider, false);
    mOffsetLabel.setJustificationType(juce::Justification::centred);

    // Tolerance Slider 
    mToleranceSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    mToleranceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(mToleranceSlider);
    toleranceSliderAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, ParamIDs::Tolerance, mToleranceSlider);

    // Tolerance Label
    addAndMakeVisible(mToleranceLabel);
    mToleranceLabel.setText("Tolerance", juce::dontSendNotification);
    mToleranceLabel.attachToComponent(&mToleranceSlider, false);
    mToleranceLabel.setJustificationType(juce::Justification::centred);

    // Detune Slider 
    mDetuneSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    mDetuneSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(mDetuneSlider);
    detuneSliderAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, ParamIDs::Detune, mDetuneSlider);

    // Detune Label
    addAndMakeVisible(mDetuneLabel);
    mDetuneLabel.setText("Detune", juce::dontSendNotification);
    mDetuneLabel.attachToComponent(&mDetuneSlider, false);
    mDetuneLabel.setJustificationType(juce::Justification::centred);

    // color
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::mediumvioletred);
    getLookAndFeel().setColour(juce::Slider::trackColourId, juce::Colours::purple);
    getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::purple);
    
}

FeedbackAudioProcessorEditor::~FeedbackAudioProcessorEditor()
{
}

//==============================================================================
void FeedbackAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void FeedbackAudioProcessorEditor::resized()
{
    // bounds for components
    juce::Rectangle<int> sliderBounds (50, getHeight() / 2 - 75, 100, 150);
    
    mOffsetSlider      .setBounds(sliderBounds);
    mDetuneSlider      .setBounds(sliderBounds.translated(100, 0));
    mToleranceSlider   .setBounds(sliderBounds.translated(200, 0));
    mFeedbackGainSlider.setBounds(sliderBounds.translated(300, 0));
    mGainSlider        .setBounds(sliderBounds.translated(400, 0));
}
