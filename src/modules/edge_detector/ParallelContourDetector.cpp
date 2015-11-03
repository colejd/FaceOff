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

/**
 Constructor.
 */
ParallelContourDetector::ParallelContourDetector(cv::UMat& in, cv::Mat& out, int _subsections, int _lineThickness){
    //in.copyTo(input);
    //out.copyTo(output);
    input = in;
    output = out;
    subsections = _subsections;
    lineThickness = _lineThickness;
    
}

/**
 Runs parallel contour detection without the need for an instance of ParallelContourDetector.
 */
void ParallelContourDetector::DetectContoursParallel(cv::UMat in, cv::Mat& out,
                            const int subsections, const int lineThickness){
    
        //std::cout << "Output was empty\n";
    cv::parallel_for_(cv::Range(0, subsections), ParallelContourDetector(in, out, subsections, lineThickness));
    
}

void ParallelContourDetector::operator ()(const cv::Range &range) const{
    
    for(int i = range.start; i != range.end; i++){
        vector< vector<cv::Point> > contourData;
        vector<Vec4i> contourHierarchy;
        cv::UMat in(input, cv::Rect(0, (input.rows/subsections)*i, input.cols, input.rows/subsections));
        cv::Mat out(output, cv::Rect(0, (output.rows/subsections)*i, output.cols, output.rows/subsections) );
        try{
            findContours( in, contourData, contourHierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
            out = Scalar::all(0);
            Scalar color = Scalar(255, 255, 255, 255);
            //srand (time(NULL));
            
            //Slow draw
            //drawContours(out, contourData, -1, color, lineThickness, 8, contourHierarchy);
            
            //Faster draw
            for (vector<cv::Point> contour : contourData) {
                //if(true) color = Scalar(rand()&255, rand()&255, rand()&255);
                polylines(out, contour, true, color, lineThickness, 8);
            }
            
        }
        catch(...){
            printf("ParallelContourDetector error!\n");
        }
        
        //out.copyTo(output(cv::Rect(0, (output.rows/subsections)*i, output.cols, output.rows/subsections)));
        
        in.release();
        out.release();
    }
    
}

void ParallelContourDetector::DetectContours(cv::UMat& in, cv::UMat& out, const int lineThickness){
    vector< vector<cv::Point> > contourData;
    vector<Vec4i> contourHierarchy;
    findContours( in, contourData, contourHierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
    out = Scalar::all(0);
    Scalar color = Scalar(255, 255, 255, 255);
    //srand (time(NULL));
    for (vector<cv::Point> contour : contourData) {
        //if(true) color = Scalar(rand()&255, rand()&255, rand()&255);
        polylines(out, contour, true, color, lineThickness, 8);
    }
}