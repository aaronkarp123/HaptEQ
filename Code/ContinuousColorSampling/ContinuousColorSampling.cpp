#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

/**
 * @function main
 */
int main( int argc, char** argv )
{
    VideoCapture cap(argv[1]); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;
    
    int frame_num = 1;
    
    Mat b_hist_cum, g_hist_cum, r_hist_cum;
    
    for(;;)
    {
        Mat src;
        cap >> src; // get a new frame from camera
        Mat dst;
        
        /// Separate the image in 3 places ( B, G and R )
        vector<Mat> bgr_planes;
        split( src, bgr_planes );
        
        /// Establish the number of bins
        int histSize = 256;
        
        /// Set the ranges ( for B,G,R) )
        float range[] = { 0, 256 } ;
        const float* histRange = { range };
        
        bool uniform = true; bool accumulate = false;
        
        Mat b_hist, g_hist, r_hist;
        
        /// Compute the histograms:
        calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
        calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
        calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );
        
        if (frame_num == 1){
            b_hist_cum = b_hist;
            g_hist_cum = g_hist;
            r_hist_cum = r_hist;
        }
        else{
            b_hist_cum = (((frame_num - 1)/frame_num)*b_hist_cum + (1/frame_num)*b_hist) / 2;
            g_hist_cum = (((frame_num - 1)/frame_num)*g_hist_cum + (1/frame_num)*g_hist) / 2;
            r_hist_cum = (((frame_num - 1)/frame_num)*r_hist_cum + (1/frame_num)*r_hist) / 2;
        }
        
        // Draw the histograms for B, G and R
        int hist_w = 512; int hist_h = 400;
        int bin_w = cvRound( (double) hist_w/histSize );
        
        Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
        
        /*//single frame
        /// Normalize the result to [ 0, histImage.rows ]
        normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
        normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
        normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
        
        /// Draw for each channel
        for( int i = 1; i < histSize; i++ )
        {
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                 Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                 Scalar( 255, 0, 0), 2, 8, 0  );
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                 Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                 Scalar( 0, 255, 0), 2, 8, 0  );
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                 Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                 Scalar( 0, 0, 255), 2, 8, 0  );
        }*/
        
        //cumulative
        /// Normalize the result to [ 0, histImage.rows ]
        normalize(b_hist_cum, b_hist_cum, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
        normalize(g_hist_cum, g_hist_cum, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
        normalize(r_hist_cum, r_hist_cum, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
        
        /// Draw for each channel
        for( int i = 1; i < histSize; i++ )
        {
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist_cum.at<float>(i-1)) ) ,
                 Point( bin_w*(i), hist_h - cvRound(b_hist_cum.at<float>(i)) ),
                 Scalar( 255, 0, 0), 2, 8, 0  );
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist_cum.at<float>(i-1)) ) ,
                 Point( bin_w*(i), hist_h - cvRound(g_hist_cum.at<float>(i)) ),
                 Scalar( 0, 255, 0), 2, 8, 0  );
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist_cum.at<float>(i-1)) ) ,
                 Point( bin_w*(i), hist_h - cvRound(r_hist_cum.at<float>(i)) ),
                 Scalar( 0, 0, 255), 2, 8, 0  );
        }
        
        /// Display
        namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE );
        imshow("calcHist Demo", histImage );
        imshow("video", src);
        
        if(waitKey(30) >= 0) break;
        frame_num++;
    }
    
    return 0;
}