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
class BasicOscillatorAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    BasicOscillatorAudioProcessorEditor (BasicOscillatorAudioProcessor&);
    ~BasicOscillatorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

    // add the Sliders and ComboBox to the Editor
    juce::Slider qualitySlider;
    juce::Slider frequencySlider;
    juce::Label frequencyLabel;
    juce::Label qualityLabel;
    juce::ComboBox oscWaveSelector;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> oscWaveSelectorAttachment;

    // Create a slider attachments to each slider using a unique pointer to objects of 
    // the class AudioProcessorValueTreeState::SliderAttachment. 
    //NB: we create attachments AFTER the sliders
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qualitySliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencySliderAttachment;

    BasicOscillatorAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicOscillatorAudioProcessorEditor)
};
