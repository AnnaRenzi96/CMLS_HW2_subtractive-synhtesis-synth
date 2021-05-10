/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicOscillatorAudioProcessorEditor::BasicOscillatorAudioProcessorEditor (SubtractiveSynthesisAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    // Initialize the sliders and combobox in the Editor constructor

    // Sliders
    getLookAndFeel().setColour
    (juce::Slider::thumbColourId,
        juce::Colours::darkred);

    qualitySlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    qualitySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
    qualitySlider.setRange(0.0, 1.0, 0.1);
    addAndMakeVisible(qualitySlider);

    qualityLabel.setText("Q Factor", juce::dontSendNotification);
    addAndMakeVisible(qualityLabel);

    frequencySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    frequencySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
    frequencySlider.setRange(50.0, 1500.0, 1.0);
    addAndMakeVisible(frequencySlider);

    frequencyLabel.setText("CutOff Frequency", juce::dontSendNotification);
    addAndMakeVisible(frequencyLabel);

    // ComboBox
    juce::StringArray choices{ "Sine", "Saw", "Square", "Triangle" };
    oscWaveSelector.addItemList(choices, 1); // This give to element's choices array an ID number that corresponds to their position in the array
    addAndMakeVisible(oscWaveSelector);

    // Now we need to instantiate our attachments and linking the value tree state and the UI features
    frequencySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FREQ", frequencySlider);
    qualitySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Q", qualitySlider);
    oscWaveSelectorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "OSC1WAVETYPE", oscWaveSelector);

}

BasicOscillatorAudioProcessorEditor::~BasicOscillatorAudioProcessorEditor()
{
}

//==============================================================================
void BasicOscillatorAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    //g.setFont (20.0f);
    g.fillAll(juce::Colours::rosybrown);
    g.setColour(juce::Colours::brown);

    auto centralArea = getLocalBounds().toFloat().reduced(10.0f);
    g.drawRoundedRectangle(centralArea, 5.0f, 3.0f);
}

void BasicOscillatorAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    // We place them in the correct positions
    frequencySlider.setBounds(25, 80, 100, 100);
    frequencyLabel.setBounds(20, 50, 130, 20);
    qualitySlider.setBounds(260, 80, 100, 100);
    qualityLabel.setBounds(280, 50, 130, 20);
    oscWaveSelector.setBounds(140, 200, 120, 30);
}
