//
//  CaptureInterface.h
//  FaceOff
//
//  Created by Jonathan Cole on 10/22/15.
//
//

#ifndef CaptureInterface_h
#define CaptureInterface_h

#include <memory>
#include "opencv2/opencv.hpp"

/**
 Abstract class defining common members that any deriving class will need to be
 used in a CameraCapture implementation.
 */
class CaptureBase {
protected:
    
private:
    /** Index of the device; starts at 0. */
    int deviceIndex = -1;
    /** Width in pixels of the camera capture. */
    int frameWidth = -1;
    /** Height in pixels of the camera capture. */
    int frameHeight = -1;
    
public:
    bool frameIsReady = false;
    /** Sets the device index */
    void SetDeviceIndex(const int index){
        deviceIndex = index;
    };
    /** Gets the device index */
    const int GetDeviceIndex(){
        return deviceIndex;
    }
    /** Sets the reported width in pixels of the device. */
    void SetFrameWidth(const int width){
        frameWidth = width;
    }
    /** Gets the reported width in pixels of the device. */
    const int GetFrameWidth(){
        return frameWidth;
    }
    /** Sets the reported height in pixels of the device. */
    void SetFrameHeight(const int height){
        frameHeight = height;
    }
    /** Gets the reported height in pixels of the device. */
    const int GetFrameHeight(){
        return frameHeight;
    }
    /**
     Gives a cv::Mat reference to the pixel data pulled from the device.
     You must call Update() to populate frame with data.
     */
    cv::Mat& GetLatestFrame(){
        return frame;
    }
    
    /**
     Prepares the camera for capture. You must call SetDeviceIndex,
     SetFrameWidth and SetFrameHeight here. Return true if successful,
     return false if not.
     */
    virtual bool Init(const int deviceNum) = 0;
    
    /**
     Updates frame with the camera data.
     */
    virtual void Update() = 0;
    
    /**
     Returns true if a processed frame is ready to be used by the program.
     */
    virtual const bool FrameIsReady() = 0;
    
    virtual void MarkFrameUsed(){
        frameIsReady = false;
    }
    
    //cv::Mat frame;
    cv::Mat frame;
};


#endif /* CaptureInterface_h */
