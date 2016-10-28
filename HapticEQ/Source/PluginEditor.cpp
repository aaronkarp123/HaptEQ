/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

//#include <highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace cv;
using namespace std;

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    
    VideoCapture cap(1); // open the default camera
    cap.open(1);
    _img = 0;
    
    cap >> frame;
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);
    //namedWindow("display",1);
    startTimerHz(60);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (Graphics& g)
{
    if(!cap.isOpened()){  // check if we succeeded
        VideoCapture cap(1);
        //cap = VideoCapture(0); // open the default camera
        cap >> frame;
        Mat edges;
        cv::cvtColor(frame, edges, COLOR_BGR2GRAY);
        /*Mat binaryMat = getBinaryMat(edges);
        Mat cannyMat = getCannyMat(edges);
        Mat midpointMat = getEQPointsMat(binaryMat, frame);
        */
        cv::Mat binaryMat(edges.size(), edges.type());
        //Apply thresholding
        cv::threshold(edges, binaryMat, 100, 255, cv::THRESH_BINARY);
        
        GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5);
        Canny(edges, edges, 0, 30, 3);
        imshow("Canny", edges);
        Mat midpointMat = getEQPointsMat(binaryMat, frame);
        imshow("Midpoints", midpointMat);
        waitKey(15);
        return;
    }
    else
        setSize(800, 400);
    
    //VideoCapture cap(1);
    cap >> frame;
    /* following will convert cv::Mat to CvMat data format, which can be
     used as a texture in OpenGL */
    /*if (_img == 0)
        _img = new CvMat(frame);
    CvArr *arr = _img;
    arrMat = cvGetMat(arr, &stub);
    cvimage = cvCreateMat(arrMat->rows, arrMat->cols, CV_8UC3);
    cvConvertImage(arrMat, cvimage, 0);
    //sleep function here:
    waitKey(33); //OpenCV
    //Sleep(33);   //windows.h
    //msleep(33);  //Qt
    //wait(33);    //Juce
    */
    
    
    cvShowImage("display", pFrame);
    
    Mat edges;
    cv::cvtColor(frame, edges, COLOR_BGR2GRAY);
    //Binary image
    
    cv::Mat binaryMat(edges.size(), edges.type());
    //Apply thresholding
    cv::threshold(edges, binaryMat, 100, 255, cv::THRESH_BINARY);
    
    GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5);
    Canny(edges, edges, 0, 30, 3);

    
    imshow("display", edges);
    
    waitKey(0);
    
    
    //g.fillAll (Colours::white);

    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cvimage->cols, cvimage->rows, 0,
    //             GL_BGRA_EXT /* GL_BGR */, GL_UNSIGNED_BYTE, cvimage->data.ptr);
    
}

cv::Mat NewProjectAudioProcessorEditor::getBinaryMat(cv::Mat& img)
{
    cv::Mat binaryMat(img.size(), img.type());
    return binaryMat;
}

cv::Mat NewProjectAudioProcessorEditor::getCannyMat(cv::Mat& img)
{
    Mat binaryMat = getBinaryMat(img);
    cv::threshold(img, binaryMat, 100, 150, cv::THRESH_BINARY);
    
    GaussianBlur(img, img, cv::Size(7,7), 1.5, 1.5);
    Canny(img, img, 0, 30, 3);
    return img;
}

cv::Mat NewProjectAudioProcessorEditor::getEQPointsMat(cv::Mat& binaryMat, cv::Mat& img)
{
    vector<cv::Point> nonZeroCoordinates;
    
    for (int i = 0; i < binaryMat.cols; i++ ) {
        for (int j = 0; j < binaryMat.rows; j++) {
            if (binaryMat.at<uchar>(j, i) == 0) {
                nonZeroCoordinates.push_back(cv::Point(i, j));
            }
        }
    }
    
    cout << "# of total points = " << binaryMat.size() << endl;
    cout << "# of nonZeroCoordinates = " << nonZeroCoordinates.size() << endl;
    
    vector< vector< int> > sortedPoints(img.cols);
    for (int i = 0; i < nonZeroCoordinates.size(); i++) {
        sortedPoints[nonZeroCoordinates[i].x].push_back(nonZeroCoordinates[i].y);
    }
    
    vector<cv::Point> midpoints(img.cols);
    int xVal = 0;
    for (int i=0; i < sortedPoints.size(); i++){
        int total = 0;
        int num = 0;
        for (int j=0; j < sortedPoints[i].size(); j++){
            ++num;
            total+=sortedPoints[i][j];
        }
        //cout << "For " << i << ", num = " << num << endl;
        if (num != 0){
            midpoints.push_back(cv::Point(xVal, int(total/num)));
        }
        ++xVal;
    }
    
    Mat midpointImg(img.rows, img.cols, CV_8UC1);
    //polylines(midpointImg, midpoints, true, Scalar(255)); // or perhaps 0
    int mod40 = int(img.cols/40);
    for (int i=0; i < midpoints.size(); i++){
        if (midpoints[i].x % mod40 == 0){
            circle(midpointImg, midpoints[i], 5, Scalar( 255, 0, 0 ));
        }
        else
            circle(midpointImg, midpoints[i], 1, Scalar( 255, 0, 0 ));
    }
    return midpointImg;

}

void NewProjectAudioProcessorEditor::timerCallback()
{
    repaint();
}

void NewProjectAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}