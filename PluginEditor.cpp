/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicOscillatorAudioProcessorEditor::BasicOscillatorAudioProcessorEditor (BasicOscillatorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    // Initialize the sliders and combobox in the Editor constructor

    // Sliders
    qualitySlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    qualitySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
    qualitySlider.setRange(0.0, 1.0, 0.1);
    addAndMakeVisible(qualitySlider);

    qualityLabel.setText("Q", juce::dontSendNotification);
    addAndMakeVisible(qualityLabel);

    frequencySlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    frequencySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
    frequencySlider.setRange(50.0, 20000.0, 100.0);
    addAndMakeVisible(frequencySlider);

    frequencyLabel.setText("Frequency", juce::dontSendNotification);
    addAndMakeVisible(frequencyLabel);

    // ComboBox
    juce::StringArray choices{ "Sine", "Saw", "Square", "Triang"};
    oscWaveSelector.addItemList(choices, 1); 
    addAndMakeVisible(oscWaveSelector);
    oscWaveSelectorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts,"OSC1WAVETYPE", oscWaveSelector);

    // Now we need to instantiate our attachments and linking the value tree state and the sliders
    frequencySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FREQ", frequencySlider);
    qualitySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Q", qualitySlider);

}

BasicOscillatorAudioProcessorEditor::~BasicOscillatorAudioProcessorEditor()
{
}

//==============================================================================
void BasicOscillatorAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::blueviolet);
    g.setFont (15.0f);
}

void BasicOscillatorAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    // we place them in the correct positions
    frequencySlider.setBounds(10, 80, 100, 100);
    frequencyLabel.setBounds(10, 50, 130, 20);
    qualitySlider.setBounds(200, 80, 100, 100);
    qualityLabel.setBounds(200, 50, 130, 20);
    oscWaveSelector.setBounds(10, 10, 100, 30);
}
