/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/video.hpp>
#include <vector>
#include <deque>
#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace cv;
using namespace std;


//==============================================================================
AudealizeeqAudioProcessorEditor::AudealizeeqAudioProcessorEditor (AudealizeeqAudioProcessor& p)
: AudioProcessorEditor (&p), processor (p)
{
    for (int i = 0; i < NUMBANDS; i++){
        juce::String paramID = "paramGain"+std::to_string(i);
        
        mGainSliders[i] = new Slider (Slider::LinearVertical, Slider::TextBoxBelow);
        addAndMakeVisible(mGainSliders[i]);
        
        mGainSliderAttachment[i] = new AudioProcessorValueTreeState::SliderAttachment (p.getValueTreeState(), paramID, *mGainSliders[i]);
        
        p.getValueTreeState().addParameterListener(paramID, this);
    }
    
    cap = VideoCapture(0);
    buttonFrame = cv::Mat();
    
    pMOG = new cv::BackgroundSubtractorMOG(200, 5, 0.7, 0);
    
    waitKey(15);
    
    cap >> frame;
    
    startTimerHz(60);
    
    previousMidpoints = {};
    
    previous_midpoints = {} ;
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 200);
}

AudealizeeqAudioProcessorEditor::~AudealizeeqAudioProcessorEditor()
{
    for (int i = 0; i < NUMBANDS; i++){
        mGainSliders[i] = nullptr;
        mGainSliderAttachment[i] = nullptr;
    }
}

//==============================================================================
void AudealizeeqAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::white);
    
    g.setColour (Colours::white);
    
    if(!cap.isOpened()){  // check if we succeeded
        cap = VideoCapture(1);
        return;
    }
    
    
    cap >> frame;
    cv::flip(frame, frame, -1);
    frame(cv::Rect(0,0,100,100)).copyTo(buttonFrame);
    imshow("buttonFrame", buttonFrame);
    //update the background model
    pMOG->operator()(frame, fgMaskMOG);
    //imshow("FG Mask MOG 2", fgMaskMOG);
    //Mat edges;
    //cv::cvtColor(frame, edges, COLOR_BGR2GRAY);
    /*cv::Mat binaryMat(edges.size(), edges.type());
     //Calculations with original frame
     cv::threshold(edges, binaryMat, 100, 255, cv::THRESH_BINARY);
     GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5);
     Canny(edges, edges, 0, 30, 3);
     */
    //Calculations with colorThreshold
    cv::Mat binaryMat = findChainByColor(frame);
    cv::threshold(fgMaskMOG, binaryMat, 100, 255, cv::THRESH_BINARY);
    GaussianBlur(fgMaskMOG, fgMaskMOG, cv::Size(19,19), 1.5, 1.5);
    Canny(fgMaskMOG, fgMaskMOG, 0, 30, 3);
    //imshow("Canny", edges);
    line(frame, cv::Point(frame.cols, frame.rows/2), cv::Point(0, frame.rows/2), Scalar(0,0,0), 1, 8, 0);
    line(frame, cv::Point(frame.cols, frame.rows/4), cv::Point(0, frame.rows/4), Scalar(0,0,0), 1, 8, 0);
    line(frame, cv::Point(frame.cols, 3*frame.rows/4), cv::Point(0, 3*frame.rows/4), Scalar(0,0,0), 1, 8, 0);
    line(frame, cv::Point(frame.cols/2, frame.rows), cv::Point(frame.cols/2, 0), Scalar(0,0,0), 1, 8, 0);
        line(frame, cv::Point(frame.cols/4, frame.rows), cv::Point(frame.cols/4, 0), Scalar(0,0,0), 1, 8, 0);
        line(frame, cv::Point(3*frame.cols/4, frame.rows), cv::Point(3*frame.cols/4, 0), Scalar(0,0,0), 1, 8, 0);
    
    imshow("Original", frame);
    
    //ArcLength Detection
    vector<vector<cv::Point> > contours;
    vector<Vec4i> hierarchy;
    Mat adjusted_contours(frame.rows, frame.cols, CV_8UC1, Scalar(255, 255,255));
    findContours(fgMaskMOG, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, cv::Point(0,0));
    double area0 = 0;
    RotatedRect box;
    //double aspect_ratio;
    for (unsigned int i=0; i<contours.size(); i++){
        // Find the minimum area enclosing bounding box
        box = minAreaRect(contours[i]);
        // Find the minimum area enclosing circle
        Point2f center, vtx[4];
        float radius = 0;
        minEnclosingCircle(contours[i], center, radius);
        box.points(vtx);
        if (radius > 400)
            drawContours(adjusted_contours, contours, i, Scalar(0,0,0), 2, 8);
        
        /*aspect_ratio = float(box.size.width)/box.size.height;
         if (aspect_ratio > 10 and box.size.area() > 1000)
         drawContours(arc_contours, contours, i, Scalar(0,0,0), 2, 8);
         else*/ if (hierarchy[i][3] < 0){
             area0 = arcLength(contours[i], false);
             //if (area0 > 1800){
             //    drawContours(adjusted_contours, contours, i, Scalar(150,150,0), 2, 8);
             //}
         }
         else{
             area0 = arcLength(contours[i], true);
             //if (area0 < 300){
             //    drawContours(adjusted_contours, contours, i, Scalar(0,0,0), 2, 8);
             //}
         }
    }
    
    imshow("adjusted_contours", adjusted_contours);
    vector<float> midpointVec = getEQPointsVec(adjusted_contours, frame);
    if (midpointVec.size() <= 0){
        cout << "No Midpoints" << endl;
        return;
    }
    for (int i = 0; i < 40; i++){
        AudioProcessorParameter* param = processor.getValueTreeState().getParameter(processor.getParamID(i));
        param->beginChangeGesture();
        float delta = midpointVec[i];
        param->setValueNotifyingHost(delta);
        param->endChangeGesture();
        circle(adjusted_contours, cv::Point(i * (frame.cols % 40), delta), 5, Scalar( 255, 0, 0 ));
    }
    
    //waitKey(15);
}

void AudealizeeqAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    Rectangle<int> box (getLocalBounds());
    box.setWidth(box.getWidth() / 40.);
    for (int i = 0; i < NUMBANDS; i++){
        mGainSliders[i]->setBounds(box);
        box.setX(box.getRight());
    }
    
}

void AudealizeeqAudioProcessorEditor::parameterChanged(const juce::String &parameterID, float newValue){
    ((AudealizeeqAudioProcessor*)getAudioProcessor())->parameterChanged(parameterID);
}

cv::Mat AudealizeeqAudioProcessorEditor::getBinaryMat(cv::Mat& img)
{
    cv::Mat binaryMat(img.size(), img.type());
    return binaryMat;
}

cv::Mat AudealizeeqAudioProcessorEditor::getCannyMat(cv::Mat& img)
{
    Mat binaryMat = getBinaryMat(img);
    cv::threshold(img, binaryMat, 100, 150, cv::THRESH_BINARY);
    
    GaussianBlur(img, img, cv::Size(7,7), 1.5, 1.5);
    Canny(img, img, 0, 100, 3);
    return img;
}

cv::Mat AudealizeeqAudioProcessorEditor::findChainByColor(cv::Mat& imgOriginal)
{
    /*//Gold Chain
     int iLowH = 0;
     int iHighH = 179;
     
     int iLowS = 0;
     int iHighS = 255;
     
     int iLowV = 0;
     int iHighV = 65;
     */
    
    /*//Red Tape
     int iLowH = 130;
     int iHighH = 179;
     
     int iLowS = 50;
     int iHighS = 255;
     
     int iLowV = 0;
     int iHighV = 255;
     */
    
    /*//Green Chain
     LowH: 30  HighH: 99
     LowS: 52  HighS: 255
     LowV: 0  HighV: 255
     */
    int iLowH = 30;
    int iHighH = 99;
    
    int iLowS = 52;
    int iHighS = 255;
    
    int iLowV = 0;
    int iHighV = 255;
    
    /*//Aaron's Hand
     LowH: 0  HighH: 179
     LowS: 48  HighS: 163
     LowV: 50  HighV: 255
     */
    Mat imgHSV;
    
    cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
    
    Mat imgThresholded;
    
    inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
    
    /*//Useful for blocks, not for chain detection
     //morphological opening (remove small objects from the foreground)
     erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, cv::Size(5, 5)) );
     dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, cv::Size(5, 5)) );
     
     //morphological closing (fill small holes in the foreground)
     dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, cv::Size(5, 5)) );
     erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, cv::Size(5, 5)) );
     */
    return imgThresholded;
}

vector<float> AudealizeeqAudioProcessorEditor::getEQPointsVec(cv::Mat& binaryMat, cv::Mat& img)
{
    vector<cv::Point> nonZeroCoordinates;
    
    for (int i = 0; i < binaryMat.cols; i++ ) {
        for (int j = 0; j < binaryMat.rows; j++) {
            if (binaryMat.at<uchar>(j, i) == 0) {
                nonZeroCoordinates.push_back(cv::Point(i, j));
            }
        }
    }
    
    vector< vector< int> > sortedPoints(img.cols);
    for (int i = 0; i < nonZeroCoordinates.size(); i++) {
        sortedPoints[nonZeroCoordinates[i].x].push_back(nonZeroCoordinates[i].y);
    }
    
    int height = img.rows;
    
    vector<cv::Point> midpoints;
    int xVal = 0;
    
    /*//USE IFF CAMERA FRAMERATE IS HIGH
     if (previousMidpoints.size() != 0)
     for (int i=0; i < sortedPoints.size(); i++){
     int total = 0;
     int num = 0;
     int smallestY = 0;//height;
     int biggestY = 0;
     for (int j=0; j < sortedPoints[i].size(); j++){
     if (sortedPoints[i][j] < smallestY)
     smallestY = sortedPoints[i][j];
     if (sortedPoints[i][j] > biggestY)
     biggestY = sortedPoints[i][j];
     
     
     if (previousMidpoints.size() == 0){
     ++num;
     total+=sortedPoints[i][j];
     }
     else
     {
     if (abs(sortedPoints[i][j] - previousMidpoints[i].y) <= 40){
     ++num;
     total+=sortedPoints[i][j];
     }
     }
     }
     //Detecting vertical interference
     if (abs(biggestY - smallestY) > 100){
     if (previousMidpoints.size() == 0){
     vector<float> empty = {};
     return empty;
     }
     midpoints.push_back(previousMidpoints[i]);
     }
     else
     if (num != 0){
     midpoints.push_back(cv::Point(xVal, int(total/num)));
     }
     ++xVal;
     }
     else*/
    
    
    //Taking average y at each frame and compare across time
    int num_interfered = 0;
    for (int i=0; i < sortedPoints.size(); i++){
        int total = 0;
        int num = 0;
        int smallestY = height;
        int biggestY = 0;
        for (int j=0; j < sortedPoints[i].size(); j++){
            if (sortedPoints[i][j] < smallestY)
                smallestY = sortedPoints[i][j];
            if (sortedPoints[i][j] > biggestY)
                biggestY = sortedPoints[i][j];
            ++num;
            total+=sortedPoints[i][j];
        }
        
        //Detecting vertical interference
        if (abs(biggestY - smallestY) > 35){
            num_interfered++;
            if (num_interfered > 600) {
                vector<float> empty = {};
                cout << "too much interference" << endl;
                return empty;
            }
            if (previousMidpoints.size() == 0){
                midpoints.push_back(cv::Point(xVal, 0));
            }
            else
                midpoints.push_back(previousMidpoints[i]);
        }
        else if (num != 0){
            float avg = float(total/num);
            for (int j=0; j < previous_midpoints.size(); j++){
                avg += previous_midpoints[j][i].y;
            }
            avg = avg / (previous_midpoints.size() + 1);
            midpoints.push_back(cv::Point(xVal, avg));
        }
        ++xVal;
    }
    
    
    /*
     //Taking max y at each frame and compare across space
     for (int i=0; i < sortedPoints.size(); i++){
     int total = 0;
     int num = 0;
     int smallestY = height;
     int biggestY = 0;
     for (int j=0; j < sortedPoints[i].size(); j++){
     if (sortedPoints[i][j] < smallestY)
     smallestY = sortedPoints[i][j];
     if (sortedPoints[i][j] > biggestY)
     biggestY = sortedPoints[i][j];
     ++num;
     total+=sortedPoints[i][j];
     }
     
     //Detecting vertical interference
     if (abs(biggestY - smallestY) > 100){
     if (previousMidpoints.size() == 0){
     vector<float> empty = {};
     std::cout << "here";
     return empty;
     }
     midpoints.push_back(previousMidpoints[i]);
     }
     else if (num != 0){
     if (i > 0){
     if (abs(midpoints[i-1].y - biggestY) > 5)
     midpoints.push_back(previousMidpoints[i]);
     else{
     midpoints.push_back(cv::Point(xVal, biggestY));
     }
     }
     else{
     midpoints.push_back(cv::Point(xVal, biggestY));
     }
     }
     ++xVal;
     }
     */
    
    vector<float> finalMidpoints;
    int delta = 40;
    int mod40 = int((midpoints.size()-2*delta)/40);
    for (int i=delta; i < midpoints.size() - delta; i++){
        if (i % mod40 == 0){
            if(midpoints[i].y == 0)
                finalMidpoints.push_back(0.f);
            else
                finalMidpoints.push_back(float(height-midpoints[i].y)/float(height));
            
        }
    }
    
    previousMidpoints = midpoints;
    vector<cv::Point> tempVec = midpoints;
    if (previous_midpoints.size() == 0)
        previous_midpoints = {tempVec};
    else
        previous_midpoints.push_front(tempVec);
    if(previous_midpoints.size() > 2    )  // # of previous midpoints to average together
        previous_midpoints.pop_back();
    return finalMidpoints;
}

cv::Mat AudealizeeqAudioProcessorEditor::getEQPointsMat(cv::Mat& binaryMat, cv::Mat& img)
{
    vector<cv::Point> nonZeroCoordinates;
    
    for (int i = 0; i < binaryMat.cols; i++ ) {
        for (int j = 0; j < binaryMat.rows; j++) {
            if (binaryMat.at<uchar>(j, i) == 0) {
                nonZeroCoordinates.push_back(cv::Point(i, j));
            }
        }
    }
    
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
        if (num != 0){
            midpoints.push_back(cv::Point(xVal, int(total/num)));
        }
        ++xVal;
    }
    
    Mat midpointImg(img.rows, img.cols, CV_8UC1);
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

void AudealizeeqAudioProcessorEditor::timerCallback()
{
    repaint();
}
