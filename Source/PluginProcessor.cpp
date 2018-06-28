/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

static Array<float> getSimpleNoise(int numSamples)
{
    Random r = Random::getSystemRandom();
    Array<float> noise;
    
    for (int s=0; s < numSamples; s++)
    {
        noise.add((r.nextFloat() - .5)*2);
    }
    
    return noise;
    
}

static Array<float> getWhiteNoise(int numSamples) {
    
    Array<float> noise;
    
    float z0 = 0;
    float z1 = 0;
    bool generate = false;
    
    float mu = 0; // center (0)
    float sigma = 1; // spread -1 <-> 1
    
    float output = 0;
    float u1 = 0;
    float u2 = 0;
    
    Random r = Random::getSystemRandom();
    r.setSeed(Time::getCurrentTime().getMilliseconds());
    const float epsilon = std::numeric_limits<float>::min();
    
    for (int s=0; s < numSamples; s++)
    {
        
        // GENERATE ::::
        // using box muller method
        // https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
        generate = !generate;
        
        if (!generate)
            output =  z1 * sigma + mu;
        else
        {
            do
            {
                u1 = r.nextFloat();
                u2 = r.nextFloat();
            }
            while ( u1 <= epsilon );
            
            z0 = sqrtf(-2.0 * logf(u1)) * cosf(2*float(double_Pi) * u2);
            z1 = sqrtf(-2.0 * logf(u1)) * sinf(2*float(double_Pi) * u2);
            
            output = z0 * sigma + mu;
        }
        
        // NAN check ...
        jassert(output == output);
        jassert(output > -50 && output < 50);
        
        //
        noise.add(output);
        
    }
    
    return noise;
    
}



//==============================================================================
NewProjectAudioProcessor::NewProjectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    
    
     noiseAmount = new BitCrush_Parameter();
     noiseAmount->defaultValue = 0;
     noiseAmount->currentValue = 0;
     noiseAmount->name = "Noise";
     addParameter(noiseAmount);
    
    rateRedux = new BitCrush_Parameter();
    rateRedux->defaultValue = 1;
    rateRedux->currentValue = 1;
    rateRedux->name = "Rate";
    addParameter(rateRedux);
    

    bitRedux = new BitCrush_Parameter();
    bitRedux->defaultValue = 32;
    bitRedux->currentValue = 32;
    bitRedux->name = "Bits";
    addParameter(bitRedux);

    
    
}

NewProjectAudioProcessor::~NewProjectAudioProcessor()
{
}

//==============================================================================
const String NewProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NewProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NewProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NewProjectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NewProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NewProjectAudioProcessor::setCurrentProgram (int index)
{
}

const String NewProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void NewProjectAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void NewProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void NewProjectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NewProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void NewProjectAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

   
    
    int numSamples = buffer.getNumSamples();
   
    float noiseAmt = -120 + 120*(noiseAmount->getValue()/100); // dB
    float bitDepth = bitRedux->getValue();
    int rateDivide = rateRedux->getValue();
    
    // GET PARAMETERS :::::
    noiseAmt = jlimit<float>(-120, 0, noiseAmt);
    noiseAmt = Decibels::decibelsToGain(noiseAmt);
    
    
    
    // SAFETY CHECK :::: since some hosts will change buffer sizes without calling prepToPlay (Bitwig)
    if (noiseBuffer.getNumSamples() != numSamples)
    {
        noiseBuffer.setSize(2, numSamples, false, true, true); // clears
        currentOutputBuffer.setSize(2, numSamples, false, true, true); // clears
    }
    
    
    // COPY for processing ...
    currentOutputBuffer.copyFrom(0, 0, buffer.getReadPointer(0), numSamples);
    if (buffer.getNumChannels() > 1) currentOutputBuffer.copyFrom(1, 0, buffer.getReadPointer(1), numSamples);
    
    
    
    // BUILD NOISE ::::::
    {
        noiseBuffer.clear();
        
        Array<float> noise = getWhiteNoise(numSamples);
        
        // range bound
        noiseAmt = jlimit<float>(0, 1, noiseAmt);
        
        FloatVectorOperations::multiply(noise.getRawDataPointer(), noiseAmt, numSamples);
        
        // ADD the noise ...
        FloatVectorOperations::add(noiseBuffer.getWritePointer(0), noise.getRawDataPointer(), numSamples);
        FloatVectorOperations::add(noiseBuffer.getWritePointer(1), noise.getRawDataPointer(), numSamples); // STEREO
        
        // MULTIPLY MODE :::::
        // Multiply the noise by the signal ... so 0 signal -> 0 noise
        //        {
        //            FloatVectorOperations::multiply(noiseBuffer.getWritePointer(0), currentOutputBuffer.getWritePointer(0), numSamples);
        //            FloatVectorOperations::multiply(noiseBuffer.getWritePointer(1), currentOutputBuffer.getWritePointer(1), numSamples);
        //        }
        
    }
    
    
    // ADD NOISE to the incoming AUDIO ::::
    currentOutputBuffer.addFrom(0, 0, noiseBuffer.getReadPointer(0), numSamples);
    currentOutputBuffer.addFrom(1, 0, noiseBuffer.getReadPointer(1), numSamples);
    
    
    
    // RESAMPLE AS NEEDED :::::
    for (int chan=0; chan < currentOutputBuffer.getNumChannels(); chan++)
    {
        float* data = currentOutputBuffer.getWritePointer(chan);
        
        for (int i=0; i < numSamples; i++)
        {
            // REDUCE BIT DEPTH :::::
            float totalQLevels = powf(2, bitDepth);
            float val = data[i];
            float remainder = fmodf(val, 1/totalQLevels);
            
            // Quantize ...
            data[i] = val - remainder;
            
            if (rateDivide > 1)
            {
                if (i%rateDivide != 0) data[i] = data[i - i%rateDivide];
            }
        }
    }
    
   
    
    // COPY to the actual output buffer :::
    buffer.copyFrom(0, 0, currentOutputBuffer, 0, 0, numSamples);
    buffer.copyFrom(1, 0, currentOutputBuffer, 1, 0, numSamples);
    
    
}

//==============================================================================
bool NewProjectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* NewProjectAudioProcessor::createEditor()
{
    return new NewProjectAudioProcessorEditor (*this);
}

//==============================================================================
void NewProjectAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void NewProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}
