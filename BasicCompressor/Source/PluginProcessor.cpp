/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
BasicCompressorAudioProcessor::BasicCompressorAudioProcessor()
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
}

BasicCompressorAudioProcessor::~BasicCompressorAudioProcessor()
{
}

//==============================================================================
const String BasicCompressorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BasicCompressorAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool BasicCompressorAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

double BasicCompressorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BasicCompressorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int BasicCompressorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BasicCompressorAudioProcessor::setCurrentProgram (int index)
{
}

const String BasicCompressorAudioProcessor::getProgramName (int index)
{
    return String();
}

void BasicCompressorAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void BasicCompressorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    timeElapsed = 0.0;
    curCompState = OFF;
}

void BasicCompressorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BasicCompressorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void BasicCompressorAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    static int counter = 0; //to track number of samples overtime
    
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
    
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    
    
    
    //apply input gain to the whole buffer at once
    float linInGain = powf(10, curInGain/20.0);
    buffer.applyGain(linInGain);
    
    float chMagnitude; //calculate the magnitude for all of each channel (either through peak detection or RMS)
    
    //go through each channel for the current frame
    for (int ch = 0; ch < totalNumInputChannels; ++ch)
    {
        //for peak detection mode
        if (detMode == PEAK)
        {
            chMagnitude = buffer.getMagnitude(ch, 0, buffer.getNumSamples());
        }
        else if (detMode == RMS)
        {
            chMagnitude = buffer.getRMSLevel(ch, 0, buffer.getNumSamples());
        }
        
        //go through each frame for this callback
        for (int n = 0; n < buffer.getNumSamples(); ++n)
        {
            //get the current sample's sign and absolute value
            float curSample = buffer.getSample(ch, n);
            float sign = (curSample > 0) - (curSample < 0);
            float curSampleAbs = fabsf(curSample);
            
            //convert dB values to linear amplitude
            float linThreshold = powf(10, curThresh/20.0);
            float linOutGain = powf(10, curOutGain/20.0);
            
            //each sample has what I'll call a "base" (the part of the sample up to the value of the threshold) and a remainder (the part of the sample, if any, that exceeds the threshold)
            float base = (curSampleAbs > linThreshold) ? linThreshold : curSampleAbs;
            float remainder = (curSampleAbs > linThreshold) ? (curSampleAbs - linThreshold) : 0.0;
            
            //adjust ratio - convert from dB to mag
            float linRatio = 1.0 / powf(10, ((float)curRatio)/20.0);
            
            //calculate the new sample, whether or not it is compressed
            float compressedSampleVal = ( base + (remainder * linRatio) ) * sign;
            float interpVal = getInterpCompVal(chMagnitude, curSample, compressedSampleVal, linThreshold);
            
            buffer.setSample(ch, n, interpVal * linOutGain);
        }
        
        //calculate RMS after the fact and display it
        float curMag = buffer.getMagnitude(ch, 0, buffer.getNumSamples());
        curSampleVal = curMag; //the displayed power value on the GUI
    }
}
//for root-mean-square detection
//    else if (detMode == RMS)
//    {
//        //calculate linear gains
//
//        float linThreshold = powf(10, curThresh/20.0);
//        float linOutGain = powf(10, curOutGain/20.0);
//        float linRatio;
//
//        //go through each input channel
//        for (int ch = 0; ch < totalNumInputChannels; ++ch)
//        {
//            //calculate RMS of the whole buffer for this channel
//            float chRMSlevel = buffer.getRMSLevel(ch, 0, buffer.getNumSamples());
//
//            if (chRMSlevel > linThreshold)
//            {
////                linRatio = powf(10, (float)(1.0/(float)curRatio)/20.0);
//            }
//            else
//            {
//                linRatio = 1;
//            }
//            buffer.applyGain(ch, 0, buffer.getNumSamples(), curRatio *linOutGain);
//
//        }
//    }
//    else //if something went wrong, clear the buffer just to be safe
//    {
//        buffer.clear();
//    }

//==============================================================================
bool BasicCompressorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* BasicCompressorAudioProcessor::createEditor()
{
    return new BasicCompressorAudioProcessorEditor (*this);
}

//==============================================================================
void BasicCompressorAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BasicCompressorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicCompressorAudioProcessor();
}

//Use state of compressor and current attack and release values to calculate and return the properly interpolated sample
float BasicCompressorAudioProcessor::getInterpCompVal(float valToCheck, float unCompressedVal, float compressedVal, float linearThreshold)
{
    switch(curCompState)
    {
        case(ATTACK):
            if (timeElapsed < curAtkTime) //if we should still be on the attack
            {
                //update how much time has passed in the current state
                timeElapsed += 1/getSampleRate();
                //return the linearly interpolated compressed value if the uncompressed is above the threshold
                if (unCompressedVal > linearThreshold)
                {
                    return ( (timeElapsed * compressedVal) +
                            ((curAtkTime - timeElapsed) * unCompressedVal ) ) * 0.5 / curAtkTime;
                }
                else
                {
                    return unCompressedVal;
                }
                
            }
            else //otherwise, change to the next state
            {
                changeCompressorState(curCompState, ACTIVE);
                return compressedVal;
            }
            break;
            
        case(ACTIVE):
            if (valToCheck <= linearThreshold) //change state to release if the overall peak has fallen below the threshold value
            {
                changeCompressorState(curCompState, RELEASE);
            }
            
            if (unCompressedVal > linearThreshold) //still, we only compress the individual values that are above the threshold
            {
                return compressedVal;
            }
            else
            {
                return unCompressedVal;
            }
            
            break;
            
        case(RELEASE):
            if (timeElapsed < curRelTime) //if we should still be on the release
            {
                timeElapsed += 1/getSampleRate();
                //return the linearly interpolated value
                
                if (unCompressedVal > linearThreshold)
                {
                    return (((curRelTime - timeElapsed) * compressedVal) +
                            (timeElapsed * unCompressedVal)) *
                    0.5 / curRelTime;
                }
                else
                {
                    return unCompressedVal;
                }
                
            } //otherwise, change to next state
            else
            {
                changeCompressorState(curCompState, OFF);
                return unCompressedVal;
            }
            break;
            
        case(OFF):
            if (valToCheck > linearThreshold)
            {
                changeCompressorState(curCompState, ACTIVE);
            }
            
            return unCompressedVal;
            
            break;
            
        default:
            return 0.0;
            break;
    }
}
void BasicCompressorAudioProcessor::changeCompressorState(CompressorState prevCompState, CompressorState newCompState)
{
    this->curCompState = newCompState;
    this->timeElapsed = 0.0;
}
