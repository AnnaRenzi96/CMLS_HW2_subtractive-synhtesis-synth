/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//================== Osc Functions =============================================
void OscData::setWaveType(const int choice)
{
    // return std::sin (x); //Sine Wave
    // return x / MathConstants<float>::pi; // Saw Wave
    // return x < 0.0f ? -1.0f : 1.0f;  // Square Wave

    switch (choice)
    {
    case 0:
        //Sine Wave
        initialise([](float x) {return std::sin(x);});
        break;

    case 1:
        //Saw Wave
        initialise([](float x) {return x / juce::MathConstants<float>::pi;});
        break;

    case 2:
        //Square Wave
        initialise([](float x) {return x < 0.0f ? -1.0f : 1.0f;});
        break;

    case 3:
        //Triangular Wave
        initialise([](float x) {return abs(x); });
        break;

    default:
        jassertfalse; // You're not supposed to be here
        break;

    }
};


//==============================================================================
BasicOscillatorAudioProcessor::BasicOscillatorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), false)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters()),
                        lowPassFilter(juce::dsp::IIR::Coefficients<float>::makeLowPass(SAMPLE_RATE, 20000.0f, 0.1))
#endif
{
}

BasicOscillatorAudioProcessor::~BasicOscillatorAudioProcessor()
{
}

//==============================================================================
const juce::String BasicOscillatorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BasicOscillatorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BasicOscillatorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BasicOscillatorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BasicOscillatorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BasicOscillatorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BasicOscillatorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BasicOscillatorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BasicOscillatorAudioProcessor::getProgramName (int index)
{
    return {};
}

void BasicOscillatorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BasicOscillatorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // In prepare to play we need to reset our filter and initialize gain, oscillator and the filter
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();    
    
    osc_obj.prepare (spec);
    gain.prepare (spec);
    lowPassFilter.prepare(spec);
    lowPassFilter.reset();
    
    osc_obj.setFrequency (220.0f);
    gain.setGainLinear (0.10f);
}

void BasicOscillatorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BasicOscillatorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BasicOscillatorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());    

    juce::dsp::AudioBlock<float> audioBlock { buffer };

    auto& osc_param = *apvts.getRawParameterValue("OSC1WAVETYPE");
    //osc_obj.setWaveType(osc_param); //need to update wave type
    getOscillator().setWaveType(osc_param);

    osc_obj.process (juce::dsp::ProcessContextReplacing<float> (audioBlock));
    gain.process (juce::dsp::ProcessContextReplacing<float> (audioBlock));

    // Filter
    float freq = *apvts.getRawParameterValue("FREQ");
    float quality = *apvts.getRawParameterValue("Q");
    *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(SAMPLE_RATE, freq, quality);
    lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
}

//==============================================================================
bool BasicOscillatorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BasicOscillatorAudioProcessor::createEditor()
{
    return new BasicOscillatorAudioProcessorEditor (*this);
}

//==============================================================================
void BasicOscillatorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BasicOscillatorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicOscillatorAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout BasicOscillatorAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    // we push in the vector two objects using make unique template
    // OSC
    params.push_back(std::make_unique<juce::AudioParameterChoice>("OSC1WAVETYPE", "Osc 1 Wave Type", juce::StringArray{ "Sine", "Saw", "Square","Triang" }, 0));
    // Filter
    params.push_back(std::make_unique<juce::AudioParameterFloat>("FREQ", "CutOff Frequency", 50.0f, 20000.0f, 500.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Q", "Q Factor", 0.1f, 1.0f, 0.5f));

    return { params.begin(),params.end() };
};

