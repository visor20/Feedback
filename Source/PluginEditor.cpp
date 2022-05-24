/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FeedbackAudioProcessorEditor::FeedbackAudioProcessorEditor (FeedbackAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize(600, 300);

    // Gain Slider 
    mGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    mGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(mGainSlider);
    gainSliderAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "GAIN", mGainSlider);

    // Gain Label 
    addAndMakeVisible(mGainLabel);
    mGainLabel.setText("Gain Out", juce::dontSendNotification);
    mGainLabel.attachToComponent(&mGainSlider, false);
    mGainLabel.setJustificationType(juce::Justification::centred);

    // Feedback Gain Slider 
    mFeedbackGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    mFeedbackGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(mFeedbackGainSlider);
    //mFeedbackGainSlider.setSkewFactor(0.02, false);
    feedbackGainSliderAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FEEDBACK", mFeedbackGainSlider);

    // Feedback Gain Label
    addAndMakeVisible(mFeedbackGainLabel);
    mFeedbackGainLabel.setText("Feedback Gain", juce::dontSendNotification);
    mFeedbackGainLabel.attachToComponent(&mFeedbackGainSlider, false);
    mFeedbackGainLabel.setJustificationType(juce::Justification::centred);

    // Offset Slider 
    mOffsetSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    mOffsetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    mOffsetSlider.setTextValueSuffix(" +");
    addAndMakeVisible(mOffsetSlider);
    offsetSliderAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "OFFSET", mOffsetSlider);

    // Offset Label 
    addAndMakeVisible(mOffsetLabel);
    mOffsetLabel.setText("Feedback Offset", juce::dontSendNotification);
    mOffsetLabel.attachToComponent(&mOffsetSlider, false);
    mOffsetLabel.setJustificationType(juce::Justification::centred);

    // Tolerance Slider 
    mToleranceSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    mToleranceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(mToleranceSlider);
    toleranceSliderAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "TOLERANCE", mToleranceSlider);

    // Tolerance Label
    addAndMakeVisible(mToleranceLabel);
    mToleranceLabel.setText("Tolerance", juce::dontSendNotification);
    mToleranceLabel.attachToComponent(&mToleranceSlider, false);
    mToleranceLabel.setJustificationType(juce::Justification::centred);

    // Detune Slider 
    mDetuneSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    mDetuneSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(mDetuneSlider);
    detuneSliderAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DETUNE", mDetuneSlider);

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
    juce::Component::paint(g);
}

void FeedbackAudioProcessorEditor::resized()
{
    mGainSlider.setBounds(450, getHeight() / 2 - 75, 100, 150);
    mFeedbackGainSlider.setBounds(350, getHeight() / 2 - 75, 100, 150);
    mOffsetSlider.setBounds(50, getHeight() / 2 - 75, 100, 150);
    mDetuneSlider.setBounds(150, getHeight() / 2 - 75, 100, 150);
    mToleranceSlider.setBounds(250, getHeight() / 2 - 75, 100, 150);
}
