//
//  FPSCounter.cpp
//  FaceOff
//
//  Created by Jonathan Cole on 10/20/15.
//  Copyright © 2015 vemilab. All rights reserved.
//

#include "FPSCounter.h"

using namespace cv;

FPSCounter::FPSCounter() {
    text = (char*) malloc(BUF_MAX + 1);
    this->Reset();
}


FPSCounter::~FPSCounter() {
    free(text);
    this->Reset();
}


void FPSCounter::StartFrameUpdate(){
    t = (double)getTickCount();
}


void FPSCounter::EndFrameUpdate(){
    t = (double)getTickCount() - t;
    
    double tfps = getTickFrequency()/t;
    //static double avgfps = 0;
    nframes++;
    double alpha = nframes > 50 ? 0.01 : 1./nframes;
    avgfps = avgfps*(1-alpha) + tfps*alpha;
    fps = avgfps;
    
    if(nframes > 10000) nframes = 0;
}


void FPSCounter::Reset(){
    fps = 0;
    avgfps = 0;
    nframes = 0;
    t = 0;
}


double FPSCounter::GetFPS() const{
    return fps;
}


void FPSCounter::DrawToMat(cv::Mat &mat, const int topLeftX, const int topLeftY){
    cv::Point textAnchorPoint(topLeftX, topLeftY);
    DrawToMat(mat, textAnchorPoint);
    
}

void FPSCounter::DrawToMat(cv::Mat &mat, const cv::Point topLeft){
    //char text[50];
    sprintf(text, "FPS: %.2f", fps);
    //putText(edges, text, Point(100, 100), FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 255, 0));
    
    //int baseline=0;
    //cv::Size textSize = getTextSize(text, fontFace, fontScale, thickness, &baseline);
    //baseline += thickness;
    
    // Draw the background box
    /*
    rectangle(mat, topLeft + Point(0, baseline),
              topLeft + Point(textSize.width, -textSize.height),
              Scalar(0, 0, 0), -1);
    // ... and the baseline first
     */
    /*
    line(frame, topLeft + Point(0, thickness),
         topLeft + Point(textSize.width, thickness),
         Scalar(0, 0, 255));
    */
    // Draw the text
    putText(mat, text, topLeft, fontFace, fontScale, Scalar(0, 0, 255, 255), thickness, 8);
}

