/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"
#include "Equalizer.h"

#define NUMBANDS 40

//==============================================================================
/**
*/
class AudealizeeqAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    AudealizeeqAudioProcessor();
    ~AudealizeeqAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet) ;
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
    
    void parameterChanged(const juce::String &parameterID);
    
    NormalisableRange<float> getRange()
    {
        return mGainRange;
    }
    
    String getParamID(int index)
    {
        return "paramGain" + String(index);
    }
    
    AudioProcessorValueTreeState& getValueTreeState();
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudealizeeqAudioProcessor)
    
    NormalisableRange<float> mGainRange = NormalisableRange<float>(-12.0f, 12.0f, 0.0001f);
    
    std::vector<float> mFreqs = {20, 50, 83, 120, 161, 208, 259, 318, 383, 455, 537, 628, 729, 843, 971, 1114, 1273, 1452, 1652, 1875, 2126, 2406, 2719, 3070, 3462, 3901, 4392, 4941, 5556, 6244, 7014, 7875, 8839, 9917, 11124, 12474, 13984, 15675, 17566, 19682};
    
    Equalizer mEqualizer;
    
    
    ScopedPointer<AudioProcessorValueTreeState> mState;
    ScopedPointer<UndoManager>                  mUndoManager;
    
    

};


#endif  // PLUGINPROCESSOR_H_INCLUDED
