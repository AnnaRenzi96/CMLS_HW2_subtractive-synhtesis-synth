/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#define SAMPLE_RATE (44100)

class OscData : public juce::dsp::Oscillator<float>
{
public:
    void setWaveType(const int choice);

private:

};

//==============================================================================
/**
*/
class BasicOscillatorAudioProcessor  : public juce::AudioProcessor
{
public:

    OscData osc_obj;
    OscData& getOscillator() { return osc_obj; }
    juce::dsp::Gain<float> gain;

    //==============================================================================
    BasicOscillatorAudioProcessor();
    ~BasicOscillatorAudioProcessor() override;

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

    //==============================================================================
    // We are going to use the AudioProcessorValueTreeState class for implementing
    // the communication between Editor and Processor
    juce::AudioProcessorValueTreeState apvts;
    // we need to define a function where we actually associate all parameters and that
    // returns a ParameterLayout
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

private:
    // Add the DSP filter in the Processor header
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients <float>> lowPassFilter;
    float lastSampleRate;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicOscillatorAudioProcessor)
};
