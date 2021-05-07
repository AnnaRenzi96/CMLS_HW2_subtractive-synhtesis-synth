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

    juce::ComboBox oscWaveSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> oscWaveSelectorAttachment;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BasicOscillatorAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicOscillatorAudioProcessorEditor)
};
