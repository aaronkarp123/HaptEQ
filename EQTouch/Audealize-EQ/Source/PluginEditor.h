/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include <deque>
#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "LookAndFeel.h"


//==============================================================================
/**
*/
class AudealizeeqAudioProcessorEditor  : public AudioProcessorEditor, public Timer, AudioProcessorValueTreeState::Listener
{
public:
    AudealizeeqAudioProcessorEditor (AudealizeeqAudioProcessor&);
    ~AudealizeeqAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void parameterChanged(const String &parameterID, float newValue) override;
    
    void timerCallback() override;
    
    cv::Mat getBinaryMat(cv::Mat&);
    cv::Mat getCannyMat(cv::Mat&);
    cv::Mat findChainByColor(cv::Mat&);
    cv::Mat getEQPointsMat(cv::Mat&, cv::Mat&);
    
    bool buttonDetected(cv::Mat&);
    
    vector<float> getEQPointsVec(cv::Mat&, cv::Mat&);
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudealizeeqAudioProcessor& processor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudealizeeqAudioProcessorEditor)
    
    ScopedPointer<Slider> mGainSliders[NUMBANDS];
    
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> mGainSliderAttachment[NUMBANDS];
    ScopedPointer<AudioProcessorValueTreeState::Listener> mGainListener[NUMBANDS];
    
    cv::Mat frame; // object to receive a captured frame.
    cv::VideoCapture cap;  // video capture object
    cv::Mat buttonFrame;
    
    cv::Ptr<cv::BackgroundSubtractor> pMOG;
    cv::Mat fgMaskMOG; //fg mask fg mask generated by MOG2 method
    
    vector<cv::Point> previousMidpoints;
    
    std::deque<vector<cv::Point>> previous_midpoints;
    
    std::deque<bool> previousButtons;
    
    int num_times_run;
    
    String filename = "/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad";
    
    cv::VideoWriter outCapture;
    
    TextButton checkTheTimeButton;
    Label timeLabel;
    
    Audealize::AudealizeLookAndFeelDark lookAndFeel;
};


#endif  // PLUGINEDITOR_H_INCLUDED
