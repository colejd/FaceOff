//
//  ParallelContourDetector.cpp
//  FaceOff
//
//  Created by Jonathan Cole on 10/29/15.
//
//

#include "ParallelContourDetector.hpp"

using namespace cv;
using namespace std;

void ParallelContourDetector::operator ()(const cv::Range &range) const{
    
    for(int i = range.start; i < range.end; i++){
        vector< vector<cv::Point> > contourData;
        vector<Vec4i> contourHierarchy;
        cv::Mat in(src_gray, cv::Rect(0, (src_gray.rows/subsections)*i, src_gray.cols, src_gray.rows/subsections));
        cv::Mat out(out_gray, cv::Rect(0, (out_gray.rows/subsections)*i, out_gray.cols, out_gray.rows/subsections) );
        try{
            findContours( in, contourData, contourHierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
            out = Scalar::all(0);
            Scalar color = Scalar(255, 255, 255, 255);
            //srand (time(NULL));
            for (vector<cv::Point> contour : contourData) {
                //if(true) color = Scalar(rand()&255, rand()&255, rand()&255);
                polylines(out, contour, true, color, lineThickness, 8);
            }
            
        }
        catch(...){
            printf("[CVEye] ParallelContourDetector error.\n");
        }
        
        in.release();
        out.release();
    }
    
}

ParallelContourDetector::~ParallelContourDetector(){ }