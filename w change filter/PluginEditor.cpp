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
    setSize (370, 500);

    // Initialize the sliders and combobox in the Editor constructor

    // Sliders
    getLookAndFeel().setColour
    (juce::Slider::thumbColourId,
        juce::Colours::brown);

    frequencySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    frequencySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
    frequencySlider.setRange(50.0, 1500.0, 1.0);
    addAndMakeVisible(frequencySlider);

    frequencyLabel.setText("CutOff Frequency", juce::dontSendNotification);
    frequencyLabel.setFont(juce::Font(18.0f));
    addAndMakeVisible(frequencyLabel);

    // ComboBoxes
    oscLabel.setText("Wave Type", juce::dontSendNotification);
    oscLabel.setFont(juce::Font(18.0f));
    addAndMakeVisible(oscLabel);

    juce::StringArray osc_choices{ "Sine", "Saw", "Square", "Triangle" };
    oscWaveSelector.addItemList(osc_choices, 1); // This give to element's choices array an ID number that corresponds to their position in the array
    addAndMakeVisible(oscWaveSelector);

    filterLabel.setText("Filter Type", juce::dontSendNotification);
    filterLabel.setFont(juce::Font(18.0f));
    addAndMakeVisible(filterLabel);

    juce::StringArray filter_choices{"Low-Pass","Band-Pass","High-Pass"};
    filterSelector.addItemList(filter_choices, 1);
    addAndMakeVisible(filterSelector);

    // Now we need to instantiate our attachments and linking the value tree state and the UI features
    frequencySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FILTERFREQ", frequencySlider);
    oscWaveSelectorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "OSC1WAVETYPE", oscWaveSelector);
    filterSelectorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "FILTERTYPE", filterSelector);
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

    oscLabel.setBounds(135, 80, 100, 40);
    oscWaveSelector.setBounds(110, 130, 150, 30);

    frequencySlider.setBounds(35, 350, 120, 120);
    frequencyLabel.setBounds(25, 320, 140, 40);

    filterSelector.setBounds(210, 370, 120, 30);
    filterLabel.setBounds(220, 320, 140, 40);
}
