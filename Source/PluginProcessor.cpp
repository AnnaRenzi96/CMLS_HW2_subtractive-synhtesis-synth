/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//================== Osc Function =============================================
void OscData::setWaveType(const int choice, const float amp)
{   
    switch (choice)
    {
    case 0:
        //Sine Wave
        initialise([=](float x) {return ((std::sin(x)) * amp);});
        break;

    case 1:
        //Saw Wave
        initialise([=](float x) {return amp * (x / juce::MathConstants<float>::pi);});
        break;

    case 2:
        //Square Wave
        initialise([=](float x) {return amp * (x < 0.0f ? -1.0f : 1.0f);});
        break;

    case 3:
        //Triangular Wave
        initialise([=](float x) {return amp *(x += (x >= 0.0f) ? delta : -delta); });
        break;

    case 4:
        //Pinky Wave
        initialise([=](float x) {return ((std::sin(x) * std::cos(x) * std::sin(x)) * 5 * amp); });
        break;
    case 5:
        //Ponky Wave
        initialise([=](float x) {return (x*x*x* std::cos(x*5) * (x += (x >= 0.0f) ? delta : -delta) * amp / 10); });
        break;

    default:
        jassertfalse; // You're not supposed to be here
        break;

    }
};


//==============================================================================
SubtractiveSynthesisAudioProcessor::SubtractiveSynthesisAudioProcessor()
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

SubtractiveSynthesisAudioProcessor::~SubtractiveSynthesisAudioProcessor()
{
}

//==============================================================================
const juce::String SubtractiveSynthesisAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SubtractiveSynthesisAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SubtractiveSynthesisAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SubtractiveSynthesisAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SubtractiveSynthesisAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SubtractiveSynthesisAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SubtractiveSynthesisAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SubtractiveSynthesisAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SubtractiveSynthesisAudioProcessor::getProgramName (int index)
{
    return {};
}

void SubtractiveSynthesisAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SubtractiveSynthesisAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // In prepare to play we need to reset our filter and initialize gain, oscillator and the filter
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();    
    
    osc_obj.prepare (spec);
    lowPassFilter.prepare(spec);
    lowPassFilter.reset();
}

void SubtractiveSynthesisAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SubtractiveSynthesisAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void SubtractiveSynthesisAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());    

    //********************************************************************************************//
    // 9) implement the actual change in note velocity: for every message we are going to create a new
    // message equal to the original one except for the velocity, add it to a buffer of midi messages and
    // then we swap the modified midi buffer with the original one
    buffer.clear();
    juce::MidiBuffer processedMidi;
    int time;
    juce::MidiMessage m;
    
    for (juce::MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);) {
        if (m.isNoteOn())
        {
            amp = 0.1;
            car_freq = m.getMidiNoteInHertz(m.getNoteNumber());
            osc_obj.setFrequency(car_freq);
        }
        else if (m.isNoteOff())
        {
            amp = 0;
        }
        else if (m.isAftertouch())
        {
        }
        else if (m.isPitchWheel())
        {
        }
        processedMidi.addEvent (m, time);
    }   
    midiMessages.swapWith(processedMidi);        
        
    juce::dsp::AudioBlock<float> audioBlock { buffer };

    auto& osc_param = *apvts.getRawParameterValue("OSC1WAVETYPE");
    //need to update wave type
    getOscillator().setWaveType(osc_param, amp);

    osc_obj.process (juce::dsp::ProcessContextReplacing<float> (audioBlock));

    // Filter
    float freq = *apvts.getRawParameterValue("FREQ");
    float quality = *apvts.getRawParameterValue("Q");
    *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(SAMPLE_RATE, freq, quality);
    lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
}

//==============================================================================
bool SubtractiveSynthesisAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SubtractiveSynthesisAudioProcessor::createEditor()
{
    return new BasicOscillatorAudioProcessorEditor (*this);
}

//==============================================================================
void SubtractiveSynthesisAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SubtractiveSynthesisAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SubtractiveSynthesisAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout SubtractiveSynthesisAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    // we push in the vector two objects using make unique template
    // OSC
    params.push_back(std::make_unique<juce::AudioParameterChoice>("OSC1WAVETYPE", "Osc 1 Wave Type", juce::StringArray{ "Sine", "Saw", "Square", "Triangle", "Pinky", "Ponky" }, 0));
    // Filter
    params.push_back(std::make_unique<juce::AudioParameterFloat>("FREQ", "CutOff Frequency", 50.0f, 1500.0f, 500.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Q", "Q Factor", 0.1f, 1.0f, 0.5f));

    return { params.begin(),params.end() };
};

