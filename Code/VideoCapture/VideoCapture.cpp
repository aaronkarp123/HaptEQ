#include "opencv2/opencv.hpp"

#define PI 3.1415926

using namespace cv;
using namespace std;

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

int main(int, char**)
{
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;
    
    Mat edges;
    namedWindow("edges",1);
    for(;;)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera
        cvtColor(frame, edges, COLOR_BGR2GRAY);
        //Binary image
        
        cv::Mat binaryMat(edges.size(), edges.type());
        //Apply thresholding
        cv::threshold(edges, binaryMat, 100, 250, cv::THRESH_BINARY);
        
        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        Canny(edges, edges, 0, 100, 3);
        imshow("edges", edges);
        
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        findContours(edges, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, Point(0,0));
        /*for (unsigned int i=0; i<contours.size(); i++)
            if (hierarchy[i][3] >= 0)   //has parent, inner (hole) contour of a closed edge (looks good)
                drawContours(edges, contours, i, Scalar(255, 0, 0), 1, 8);
         imshow("special_contours", edges);
         */
        for (unsigned int i=0; i<contours.size(); i++){
            if (hierarchy[i][3] < 0){
                double area0 = arcLength(contours[i], false);
                if (area0 > 2000){
                    std::cout << "contourArea(contours["<< i << "]) = " << area0 << endl;
                    drawContours(frame, contours, i, Scalar(255,0,0), 1, 8);
                }
            }
        }
        imshow("special_contours", frame);
        cout << endl;
        
        char key = waitKey(30);
        if (key == ' '){
            
            
            
            vector<Point> nonZeroCoordinates(binaryMat.cols*binaryMat.rows);
            
            for (int i = 0; i < binaryMat.cols; i++ ) {
                for (int j = 0; j < binaryMat.rows; j++) {
                    if (binaryMat.at<uchar>(j, i) == 0) {
                        nonZeroCoordinates.push_back(Point(i, j));
                    }
                }
            }
            
            vector< vector< int> > sortedPoints(edges.cols);
            for (int i = 0; i < nonZeroCoordinates.size(); i++) {
                sortedPoints[nonZeroCoordinates[i].x].push_back(nonZeroCoordinates[i].y);
            }
            
            
            vector<Point> midpoints(edges.cols);
            int xVal = 0;
            for (auto i = sortedPoints.begin(); i != sortedPoints.end(); ++i){
                int total = 0;
                int num = 0;
                for (auto j = (*i).begin(); j != (*i).end(); ++j){
                    ++num;
                    total += *j;
                }
                if (num != 0){
                    midpoints.push_back(Point(xVal, int(total/num)));
                }
                ++xVal;
            }
            
            Mat midpointImg(frame.rows, frame.cols, CV_8UC1);
            //polylines(midpointImg, midpoints, true, Scalar(255)); // or perhaps 0
            int count = 1;
            int mod40 = int(frame.cols/40);
            for (auto i = midpoints.begin(); i != midpoints.end(); ++i){
                if ((*i).x % mod40 == 0){
                    circle(midpointImg, *i, 5, Scalar( 255, 0, 0 ));
                }
                else
                    circle(midpointImg, *i, 1, Scalar( 255, 0, 0 ));
            }
            
            vector<int> compression_params; //vector that stores the compression parameters of the image
            compression_params.push_back(CV_IMWRITE_JPEG_QUALITY); //specify the compression technique
            compression_params.push_back(100); //specify the compression quality
            bool bSuccess = imwrite("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/Code/VideoCapture/TestImage.jpg", frame, compression_params); //write the image to file
            if ( !bSuccess )
                std::cout << "ERROR : Failed to save the image";
            
            bSuccess = imwrite("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/Code/VideoCapture/TestImageContour.jpg", edges, compression_params); //write the image to file
            if ( !bSuccess )
                std::cout << "ERROR : Failed to save the contour image";
            
            bSuccess = imwrite("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/Code/VideoCapture/TestImageBinary.jpg", binaryMat, compression_params); //write the image to file
            if ( !bSuccess )
                std::cout << "ERROR : Failed to save the binary image";
            
            bSuccess = imwrite("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/Code/VideoCapture/TestImageMidpoints.jpg", midpointImg, compression_params); //write the image to file
            if ( !bSuccess )
                std::cout << "ERROR : Failed to save the midpoint image";
            
            // Hough transform for line detection
            cv ::Mat result( edges.rows ,edges. cols,CV_8U ,cv:: Scalar(255 ));
            frame .copyTo( result);
            std ::vector< cv::Vec2f > lines;
            cv ::HoughLines( edges, lines ,
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
            bSuccess = imwrite("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/Code/VideoCapture/TestImageHough.jpg", result, compression_params); //write the image to file
            if ( !bSuccess )
                std::cout << "ERROR : Failed to save the hough image";
            
            //Probabalistic Hough
            // Create LineFinder instance
            LineFinder finder ;
            
            // Set probabilistic Hough parameters
            finder .setLineLengthAndGap( 100, 20);
            finder .setMinVote( 80);
            
            // Detect lines and draw them
            std ::vector< cv::Vec4i > linesP = finder.findLines (edges);
            finder .drawDetectedLines( frame);
            bSuccess = imwrite("/Users/aaronkarp/Documents/Research/Summer2016/GraphingPad/Code/VideoCapture/TestImageHoughP.jpg", frame, compression_params); //write the image to file
            if ( !bSuccess )
                std::cout << "ERROR : Failed to save the houghp image";
        }
        if(key == 27) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
