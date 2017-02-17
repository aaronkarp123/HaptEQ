#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>       /* time */

using namespace cv;
using namespace std;

Mat src; Mat src_gray;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);

/// Function header
void thresh_callback(int, void* );

/** @function main */
int main( int argc, char** argv )
{
    /// Load source image and convert it to gray
    src = imread( argv[1], 1 );
    
    /// Convert image to gray and blur it
    cvtColor( src, src_gray, CV_BGR2GRAY );
    blur( src_gray, src_gray, Size(3,3) );
    
    /// Create Window
    char* source_window = "Source";
    namedWindow( source_window, CV_WINDOW_AUTOSIZE );
    imshow( source_window, src );
    
    //createTrackbar( " Canny thresh:", "Source", &thresh, max_thresh, thresh_callback );
//    thresh_callback( 0, 0 );
    
    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    
    /// Detect edges using canny
    Canny( src_gray, canny_output, thresh, thresh*2, 3 );
    /// Find contours
    findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0, 0) );
    
    /// Draw contours
    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ )
    {
        //Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, i, Scalar(255,255,255), 2, 8, hierarchy, 0, Point() );
    }
    
    /// Show in a window
    namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
    imshow( "Contours", drawing );
    
    
    /*vector<cv::Point> nonZeroCoordinates;
    nonZeroCoordinates.push_back(cv::Point(0,0));
    Mat drawn = Mat::zeros( drawing.size(), CV_8UC3 );
    srand (time(NULL));
    for (int i = 0; i < drawing.cols; i++ ) {
        for (int j = 0; j < drawing.rows; j++) {
            if (drawing.at<uchar>(j, i) > 0) {
                //nonZeroCoordinates.insert(nonZeroCoordinates.begin() + (rand() % nonZeroCoordinates.size()), cv::Point(i, j));
                nonZeroCoordinates.push_back(cv::Point(i, j));
            }
        }
    }
    namedWindow("Drawn",1);
    int num = 0;
    cv::Point new_point;
    Vec3b color;
    char key;
    for(;;)
    {
        num ++;
        if (nonZeroCoordinates.size() <= 0){
            cout << "end" << endl;
            break;
        }
        new_point = nonZeroCoordinates.back();
        nonZeroCoordinates.pop_back();
        color = src.at<Vec3b>(new_point);
        drawn.at<Vec3b>(new_point) = color;
        if (num % 100 == 0){
            imshow( "Drawn", drawn);
            key = waitKey(30);
        }
    }
    imshow( "Drawn", drawn);
    */
    Mat drawn = Mat::zeros( drawing.size(), CV_8UC3 );
    vector<Mat> subregions;
    // contours_final is as given above in your code
    for (int i = 0; i < contours.size(); i++)
    {
        // Get bounding box for contour
        Rect roi = boundingRect(contours[i]); // This is a OpenCV function
        
        // Create a mask for each contour to mask out that region from image.
        Mat mask = Mat::zeros(src.size(), CV_8UC1);
        drawContours(mask, contours, i, Scalar(255), CV_FILLED); // This is a OpenCV function
        
        // At this point, mask has value of 255 for pixels within the contour and value of 0 for those not in contour.
        
        // Extract region using mask for region
        Mat contourRegion;
        Mat imageROI;
        src.copyTo(imageROI, mask); // 'image' is the image you used to compute the contours.
        contourRegion = imageROI(roi);
        // Mat maskROI = mask(roi); // Save this if you want a mask for pixels within the contour in contourRegion.
        
        // Store contourRegion. contourRegion is a rectangular image the size of the bounding rect for the contour
        // BUT only pixels within the contour is visible. All other pixels are set to (0,0,0).
        subregions.push_back(contourRegion);
        
    }
    char key;
    for(;;)
    {
        if (subregions.size() <= 0){
            cout << "end" << endl;
            break;
        }
        Mat cont = subregions.back();
        subregions.pop_back();
        cont.copyTo(drawn.rowRange(1, 1+cont.rows).colRange(3, 3+cont.cols));
        imshow( "Drawn", drawn);
        key = waitKey(30);
    }
    
    waitKey(0);
    
    return(0);
}

/** @function thresh_callback */
void thresh_callback(int, void* )
{
    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    
    /// Detect edges using canny
    Canny( src_gray, canny_output, thresh, thresh*2, 3 );
    /// Find contours
    findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0, 0) );
    
    /// Draw contours
    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ )
    {
        //Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, i, Scalar(255,255,255), 2, 8, hierarchy, 0, Point() );
    }
    
    /// Show in a window
    namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
    imshow( "Contours", drawing );
    
    
    vector<cv::Point> nonZeroCoordinates;
    Mat drawn = Mat::zeros( canny_output.size(), CV_8UC3 );
    
    for (int i = 0; i < drawing.cols; i++ ) {
        for (int j = 0; j < drawing.rows; j++) {
            if (drawing.at<uchar>(j, i) != 0) {
                nonZeroCoordinates.push_back(cv::Point(i, j));
            }
        }
    }
    
    for(;;)
    {
        if (nonZeroCoordinates.size() <= 0){
            cout << "here" << endl;
            break;
        }
        cv::Point new_point = nonZeroCoordinates.back();
        nonZeroCoordinates.pop_back();
        Vec3b color = src.at<Vec3b>(new_point);
        drawn.at<Vec3b>(new_point) = color;
        imshow( "Drawn", drawn);
    }
}
