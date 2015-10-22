#ifndef __performance__FPSCounter__
#define __performance__FPSCounter__

//
//  FPSCounter.h
//  FaceOff
//
//  Created by Jonathan Cole on 10/20/15.
//  Copyright © 2015 vemilab. All rights reserved.
//

#include "opencv2/opencv.hpp"


/**
 Manages an internal FPS counter, and can draw to a mat if requested.
 Timing code appropriated from https://github.com/Itseez/opencv/blob/3.0-ocl-tp2/samples/cpp/ufacedetect.cpp
 */
class FPSCounter{
    
public:
    FPSCounter();
    ~FPSCounter();
    
    void Reset();
    
    /** Get the calculated FPS. */
    double GetFPS() const;
    
    /** Call above code you want to time. */
    void StartFrameUpdate();
    
    /** Call after code you want to time. */
    void EndFrameUpdate();
    
    /**
     Draws a box with the FPS represented as text into the specified mat, using
     (topLeftX, topLeftY) as the top-left corner of the drawn box.
     Lifted from http://docs.opencv.org/modules/core/doc/drawing_functions.html
     */
    void DrawToMat(cv::Mat &mat, const int x, const int y);
    
    /**
     Draws a box with the FPS represented as text into the specified mat, using topLeft
     as the top-left corner of the drawn box.
     Lifted from http://docs.opencv.org/modules/core/doc/drawing_functions.html
     */
    void DrawToMat(cv::Mat &mat, const cv::Point topLeft);
    
    /** Font used in DrawToMat(). */
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    
    /** Size of font used in DrawToMat(). */
    double fontScale = 0.75;
    
    /** Thickness of font used in DrawToMat(). */
    int thickness = 2;
    
    const int BUF_MAX = 50;
    char * text;
    
    
private:
    double fps;
    double avgfps = 0;
    
    double t;
    int nframes = 0;
    
};





#endif