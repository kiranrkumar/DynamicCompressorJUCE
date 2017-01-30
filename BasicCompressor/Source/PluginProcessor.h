/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

enum DetectionMode
{
    PEAK,
    RMS
};

enum CompressorState
{
    ATTACK,
    ACTIVE,
    RELEASE,
    OFF
};
//==============================================================================
/**
 */
class BasicCompressorAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    BasicCompressorAudioProcessor();
    ~BasicCompressorAudioProcessor();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    
#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
    
    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;
    
    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    //==============================================================================
    const String getName() const override;
    
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;
    
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;
    
    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    float curThresh, curInGain, curOutGain;
    int curRatio;
    float curAtkTime, curRelTime;
    float curSampleVal;
    DetectionMode detMode;
    float timeElapsed; //for attack and release times
    CompressorState curCompState;
    
    float getInterpCompVal(float valToCheck, float unCompressedVal, float compressedVal, float linearThreshold);
    void changeCompressorState(CompressorState prevCompState, CompressorState newCompState);
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicCompressorAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
