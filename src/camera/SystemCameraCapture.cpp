//
//  SystemCameraCapture.cpp
//  FaceOff
//
//  Created by Jonathan Cole on 10/22/15.
//
//

#include "SystemCameraCapture.hpp"

bool SystemCameraCapture::Init(const int deviceNum){
    SetDeviceIndex(deviceNum);
    //Return false if creating the capture failed
    cap = VideoCapture();
    cap.open(GetDeviceIndex());
    
    if(cap.isOpened()){
        SetFrameWidth(cap.get(CV_CAP_PROP_FRAME_WIDTH));
        SetFrameHeight(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
    }
    
    return cap.isOpened();
}

void SystemCameraCapture::Update(){
    //If the frame is empty, try to fill it from the capture. Sometimes OpenCV gives garbage
    //data, so we wait until this operation succeeds before continuing.
    
    if(cap.isOpened()){
        bool waitIfEmpty = false;
        
        if(waitIfEmpty && frame.empty()){
            //Wait until the frame gets filled by actual camera data.
            int maxWaitIterations = 50;
            int waitIterations = 0;
            while(frame.empty() || waitIterations < maxWaitIterations){
                cap >> frame;
                waitIterations += 1;
            }
        }
        //Otherwise just read from cap into the frame.
        else{
            cap >> frame;
        }
    }
    
}