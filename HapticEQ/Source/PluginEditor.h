/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class NewProjectAudioProcessorEditor  : public AudioProcessorEditor, public Timer
{
public:
    NewProjectAudioProcessorEditor (NewProjectAudioProcessor&);
    ~NewProjectAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void timerCallback() override;
    
    cv::Mat getBinaryMat(cv::Mat&);
    cv::Mat getCannyMat(cv::Mat&);
    cv::Mat getEQPointsMat(cv::Mat&, cv::Mat&);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NewProjectAudioProcessor& processor;
    
    cv::Mat frame; // object to receive a captured frame.
    cv::VideoCapture cap;  // video capture object
    CvMat *_img;
    CvMat *arrMat, *cvimage, stub;
    
    CvCapture* pCapture;
    IplImage* pFrame;
    
    TextButton checkTheTimeButton;
    Label timeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};

#endif  // PLUGINEDITOR_H_INCLUDED
