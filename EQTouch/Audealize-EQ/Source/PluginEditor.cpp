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
        
        mGainSliders[i] = new Slider (Slider::LinearVertical, Slider::NoTextBox);
        addAndMakeVisible(mGainSliders[i]);
        mGainSliderAttachment[i] = new AudioProcessorValueTreeState::SliderAttachment (p.getValueTreeState(), paramID, *mGainSliders[i]);
        
        p.getValueTreeState().addParameterListener(paramID, this);
    }
    LookAndFeel::setDefaultLookAndFeel (&lookAndFeel);

    cap = VideoCapture(1);
    buttonFrame = cv::Mat();
    
    pMOG = new cv::BackgroundSubtractorMOG(200, 5, 0.7, 0);
    
    waitKey(15);
    
    cap >> frame;
    
    startTimerHz(60);
    
    previousMidpoints = {};
    
    previous_midpoints = {} ;
    
    previousButtons = {};
    
    //outCapture = cv::VideoWriter(filename, CV_FOURCC('M','J','P','G'), 30, cv::Size(1989,1680));
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 200);
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
    g.fillAll (Colour(0xff2f2f2f));
    
    g.setColour (Colours::white);
    
    if(!cap.isOpened()){  // check if we succeeded
        cap = VideoCapture(1);
        return;
    }
    
    cv::Mat tempframe;
    cap >> tempframe;
    cv::flip(tempframe, tempframe, -1);
    //Check button position
    frame = tempframe(cv::Rect(100, 120, tempframe.cols - 100, tempframe.rows - 120));
    frame(cv::Rect(frame.cols-210,frame.rows-80,70,60)).copyTo(buttonFrame);
    imshow("Captured Image", frame);
    imshow("Button", buttonFrame);
    bool button_detected = buttonDetected(buttonFrame);
    if (previousButtons.size() == 0)
        previousButtons = {button_detected};
    else
        previousButtons.push_front(button_detected);
    if(previousButtons.size() > 7)  // # of previous buttons to average together
        previousButtons.pop_back();
    double buttonAvg = 0;
    for (int i = 0; i < previousButtons.size(); i++){
        if (previousButtons[i])
            buttonAvg += 1;
        else
            buttonAvg -= 1;
    }
    buttonAvg /= previousButtons.size();
    if (buttonAvg > 0)
        button_detected = true;
    else
        button_detected = false;
    if (!button_detected){
        for (int i = 0; i < 40; i++){
            AudioProcessorParameter* param = processor.getValueTreeState().getParameter(processor.getParamID(i));
            param->beginChangeGesture();
            param->setValueNotifyingHost(0.5f);
            param->endChangeGesture();
        }
        return;
    }
    //update the background model
    pMOG->operator()(frame, fgMaskMOG);
    //Calculations with colorThreshold
    cv::threshold(fgMaskMOG, findChainByColor(frame), 100, 255, cv::THRESH_BINARY);
    GaussianBlur(fgMaskMOG, fgMaskMOG, cv::Size(19,19), 1.5, 1.5);
    Canny(fgMaskMOG, fgMaskMOG, 0, 30, 3);
    
    
    /*line(frame, cv::Point(frame.cols, frame.rows/2), cv::Point(0, frame.rows/2), Scalar(0,0,0), 1, 8, 0);
    line(frame, cv::Point(frame.cols, frame.rows/4), cv::Point(0, frame.rows/4), Scalar(0,0,0), 1, 8, 0);
    line(frame, cv::Point(frame.cols, 3*frame.rows/4), cv::Point(0, 3*frame.rows/4), Scalar(0,0,0), 1, 8, 0);
    line(frame, cv::Point(frame.cols/2, frame.rows), cv::Point(frame.cols/2, 0), Scalar(0,0,0), 1, 8, 0);
        line(frame, cv::Point(frame.cols/4, frame.rows), cv::Point(frame.cols/4, 0), Scalar(0,0,0), 1, 8, 0);
        line(frame, cv::Point(3*frame.cols/4, frame.rows), cv::Point(3*frame.cols/4, 0), Scalar(0,0,0), 1, 8, 0);
    
    
    
    imshow("Original", frame);
    */
    //ArcLength Detection
    vector<vector<cv::Point> > contours;
    vector<Vec4i> hierarchy;
    Mat adjusted_contours(frame.rows, frame.cols, CV_8UC1, Scalar(255, 255,255));
    findContours(fgMaskMOG, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, cv::Point(0,0));
    //imshow("Pre-Contours", fgMaskMOG);
    RotatedRect box;
    Point2f center, vtx[4];
    float radius = 0;
    double area0 = 0;
    for (unsigned int i=0; i<contours.size(); i++){
        // Find the minimum area enclosing bounding box
        box = minAreaRect(contours[i]);
        // Find the minimum area enclosing circle
        minEnclosingCircle(contours[i], center, radius);
        box.points(vtx);
        /*// For small chain
        if (radius < 50)
            drawContours(adjusted_contours, contours, i, Scalar(0,0,0), 2, 8);
        else
            if (hierarchy[i][3] < 0){
                area0 = arcLength(contours[i], false);
                if (area0 < 200){
                
                    drawContours(adjusted_contours, contours, i, Scalar(0,0,0), 2, 8);
                }
                //else
                //    drawContours(adjusted_contours, contours, i, Scalar(100,100,100), 2, 10);
            }
            //else
            //   drawContours(adjusted_contours, contours, i, Scalar(100,100,100), 2, 10);*/
        // For large chain
        if (radius > 1000)
            drawContours(adjusted_contours, contours, i, Scalar(0,0,0), 2, 8);
        if (hierarchy[i][3] < 0){
             area0 = arcLength(contours[i], false);
             if (area0 > 1600){
                 drawContours(adjusted_contours, contours, i, Scalar(0,0,0), 2, 8);
             }
         }
        /*
         else{
             area0 = arcLength(contours[i], true);
             if (area0 < 100){
                 drawContours(adjusted_contours, contours, i, Scalar(0,0,0), 2, 8);
             }
         }
         */
    }
    
    //imshow("adjusted_contours", adjusted_contours);
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

bool AudealizeeqAudioProcessorEditor::buttonDetected(cv::Mat& img){
    int iLowH = 67;
    int iHighH = 136;
    
    int iLowS = 82;
    int iHighS = 255;
    
    int iLowV = 82;
    int iHighV = 255;
    Mat imgHSV;
    cvtColor(img, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
    Mat imgThresholded;
    inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
    if(cv::sum(imgThresholded)[0] > 30){
        return true;
    }
    
    //Change this to WHITE and take the inverse (or see if < x in conditional)
    if (previousButtons.size() > 0 && previousButtons[previousButtons.size() - 1]){
    iLowH = 0;
    iHighH = 179;
    
    iLowS = 0;
    iHighS = 41;
    
    iLowV = 0;
    iHighV = 255;
    imgHSV = Mat();
    cvtColor(img, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
    imgThresholded = Mat();
    inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
    if(cv::sum(imgThresholded)[0] < 50000){
        return true;
    }
    }
    
    vector<vector<cv::Point> > contours;
    vector<Vec4i> hierarchy;
    img = imgThresholded;
    GaussianBlur(img, img, cv::Size(19,19), 1.5, 1.5);
    Canny(img, img, 0, 30, 3);
    findContours(img, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, cv::Point(0,0));
    //imshow("buttomCanny", img);
    double area0 = 0;
    for (unsigned int i=0; i<contours.size(); i++){
        Point2f center;
        float radius = 0;
        minEnclosingCircle(contours[i], center, radius);
        if (hierarchy[i][3] < 0){
            area0 = arcLength(contours[i], false);
            if (area0 > 100){
                return true;
            }
        }
        else{
            area0 = arcLength(contours[i], true);
        }
    }
    return false;
}

void AudealizeeqAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int border = 15;
    int spacing = 3;
    Rectangle<int> box (getLocalBounds());
    box.setWidth((box.getWidth() - (2 * border) - (39 * spacing)) / 40.);
    box.setTop(border);
    box.setBottom(getBounds().getBottom() - border);
    box.setX(border);
    for (int i = 0; i < NUMBANDS; i++){
        mGainSliders[i]->setBounds(box);
        box.setX(box.getRight() + spacing);
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
    int total = 0;
    int num = 0;
    int smallestY = height;
    int biggestY = 0;
    for (int i=0; i < sortedPoints.size(); i++){
        total = 0;
        num = 0;
        smallestY = height;
        biggestY = 0;
        for (int j=0; j < sortedPoints[i].size(); j++){
            if (sortedPoints[i][j] < smallestY)
                smallestY = sortedPoints[i][j];
            if (sortedPoints[i][j] > biggestY)
                biggestY = sortedPoints[i][j];
            ++num;
            total+=sortedPoints[i][j];
        }
        
        //Detecting vertical interference
        if (abs(biggestY - smallestY) > 50){
            num_interfered++;
            if (num_interfered > 600) {
                vector<float> empty = {};
                cout << "too much interference" << endl;
                return empty;
            }
            if (previousMidpoints.size() == 0){
                midpoints.push_back(cv::Point(xVal, 0.5));
            }
            else{
                midpoints.push_back(previousMidpoints[i]);
            }
        }
        /*else if (previous_midpoints.size() > 2 && i > 0 && abs(midpoints.back().y - total/num) > 200){
            midpoints.push_back(cv::Point(xVal, midpoints.back().y));//int((midpoints.back().y * 15 + total/num )) / 16));
            cout << xVal << endl;
        }*/
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
    int delta_x = 165;
    int mod40 = int((midpoints.size()-2*delta_x)/40);
    for (int i=delta_x; i < midpoints.size() - delta_x; i++){
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
    if(previous_midpoints.size() > 3    )  // # of previous midpoints to average together
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
