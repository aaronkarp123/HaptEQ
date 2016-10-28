#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#define PI 3.1415926

//LINE FINDER CODE START

#if ! defined LINE_FINDER
#define LINE_FINDER

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

class LineFinder {
private:
    
    // original image
    cv ::Mat img;
    
    // vector containing the end points
    // of the detected lines
    std ::vector< cv::Vec4i > lines;
    
    // accumulator resolution parameters
    double deltaRho;
    double deltaTheta;
    
    // minimum number of votes that a line
    // must receive before being considered
    int minVote;
    
    // min length of a line
    double minLength;
    
    // max allowed gap along the line
    double maxGap;
    
public:
    
    // Default accumulator resolution is 1 pixel by 1 degree
    // no gap, no minimum length
    LineFinder () : deltaRho(1 ), deltaTheta( PI / 180),
    minVote (10), minLength(0. ), maxGap( 0.) {}
    
    // Set the resolution of the accumulator
    void setAccResolution( double dRho, double dTheta ) {
        deltaRho = dRho;
        deltaTheta = dTheta;
    }
    
    // Set the minimum number of votes
    void setMinVote( int minV) {
        minVote = minV;
    }
    
    // Set line length and gap
    void setLineLengthAndGap( double length, double gap ) {
        minLength = length;
        maxGap = gap;
    }
    
    // Apply probabilistic Hough Transform
    std ::vector< cv::Vec4i > findLines( cv::Mat &binary) {
        lines .clear();
        cv ::HoughLinesP( binary, lines ,
                         deltaRho , deltaTheta, minVote, minLength , maxGap);
        
        return lines;
    }
    
    // Draw the detected lines on image
    void drawDetectedLines( cv::Mat &image,
                           cv ::Scalar color = cv::Scalar (0, 0, 255)) {
        // Draw the lines
        std ::vector< cv::Vec4i >::const_iterator it2 = lines.begin ();
        
        while ( it2 != lines .end()){
            cv ::Point pt1((* it2)[0 ], (* it2)[1 ]);
            cv ::Point pt2((* it2)[2 ], (* it2)[3 ]);
            cv ::line( image, pt1 , pt2, color);
            ++ it2;
        }
    }
};

#endif

//LINE FINDER CODE END


using namespace cv;
using namespace std;

int main(int arg, char** argv) {
    
    cv::Mat image = cv:: imread( argv[1], 1 );
    if (! image.data ) {
        return 0 ;
    }
    
    cv ::namedWindow( "Original Image" );
    cv ::imshow( "Original Image" , image);
    
    // Apply Canny algorithm
    cv ::Mat contours;
    cv ::Canny( image, contours , 125 , 250 );
    
    cv ::namedWindow( "Canny edges" );
    cv ::imshow( "Canny edges" , contours);
    
    cv ::Mat result( contours.rows ,contours. cols,CV_8U ,cv:: Scalar(255 ));
    image .copyTo( result);
    
    // Hough transform for line detection
    std ::vector< cv::Vec2f > lines;
    cv ::HoughLines( contours, lines ,
                    1, PI / 180 ,              // step size
                    80);                                          // minimum number of votes
    std ::vector< cv::Vec2f >::const_iterator it = lines.begin ();
    while ( it != lines .end()) {
        
        float rho = (*it )[0];                 // first element is distance rho
        float theta = (*it )[1]; // second element is angle theta
        
        if ( theta < PI /4. || theta > 3.* PI/4. ) {     // ~vertical line
            
            // point of intersection of the line with first row
            cv ::Point pt1( rho / cos (theta), 0);
            // point of intersection of the line with last row
            cv ::Point pt2(( rho - result .rows * sin(theta )) / cos(theta ), result. rows);
            
            // draw a while line
            cv ::line( result, pt1 , pt2, cv::Scalar (255), 1);
        } else {    //~horizontal line
            // point of intersection of the line with first column
            cv ::Point pt1( 0, rho / sin( theta));
            // point of intersection of the line with last column
            cv ::Point pt2( result.cols , ( rho - result .cols * cos(theta )) / sin(theta ));
            // draw a white line
            cv ::line( result, pt1 , pt2, cv::Scalar (255), 1);
        }
        ++it;
    }
    
    cv ::namedWindow( "Detected lines with hough" );
    cv ::imshow( "Detected lines with hough" , result);
    
    // Create LineFinder instance
    LineFinder finder ;
    
    // Set probabilistic Hough parameters
    finder .setLineLengthAndGap( 100, 20);
    finder .setMinVote( 80);
    
    // Detect lines and draw them
    std ::vector< cv::Vec4i > linesP = finder.findLines (contours);
    finder .drawDetectedLines( image);
    cv ::namedWindow( "Detected Lines with HoughP" );
    cv ::imshow( "Detected Lines with HoughP" , image);
    
    cv::waitKey(80000);
    
    return 1;
}