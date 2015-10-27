//
//  CameraCapture.cpp
//  FaceOff
//
//  Created by Jonathan Cole on 10/21/15.
//
//

#include "CameraCapture.hpp"

std::thread updateThread;

CameraCapture::CameraCapture(){
    
}

CameraCapture::~CameraCapture(){
    StopUpdateThread();
}

/** Starts the camera capture, returning true if successful and false if not. */
bool CameraCapture::Init(const int deviceNum, const DEVICE_TYPE deviceType){
    SetDeviceType(deviceType);
    
    if(GetDeviceType() == DEVICE_TYPE::GENERIC){
        currentCapture = new SystemCameraCapture();
        bool success = currentCapture->Init(deviceNum);
        
        return success;
    }
    else if(GetDeviceType() == DEVICE_TYPE::PS3EYE){
        printf("PS3 Eye camera is not supported yet!\n");
        return false;
    }
    else{
        return false;
    }
}

/**
 Sets the index of the device you wish to use. Indices start at 0.
 */
void CameraCapture::SetDeviceIndex(const int index){
    currentCapture->SetDeviceIndex(index);
}

/**
 Gets the index of the device being used.
 */
const int CameraCapture::GetDeviceIndex(){
    return currentCapture->GetDeviceIndex();
}

/**
 Sets the type of the device you wish to use. See DEVICE_TYPE
 */
void CameraCapture::SetDeviceType(const DEVICE_TYPE type){
    deviceType = type;
}

/**
 Gets the type of the device you wish to use. See DEVICE_TYPE
 */
const CameraCapture::DEVICE_TYPE CameraCapture::GetDeviceType(){
    return deviceType;
}

/**
 Gets the width in pixels of the currently active capture device.
 */
const int CameraCapture::GetWidth(){
    return currentCapture->GetFrameWidth();
}

/**
 Gets the height in pixels of the currently active capture device.
 */
const int CameraCapture::GetHeight(){
    return currentCapture->GetFrameHeight();
}

/**
 Pull the camera data into frame
 */
void CameraCapture::Update(){
    currentCapture->Update();
}

std::shared_ptr<cv::Mat> CameraCapture::GetLatestFrame(){
    return currentCapture->GetLatestFrame();
}

const bool CameraCapture::FrameIsReady(){
    return currentCapture->FrameIsReady();
}

void CameraCapture::StartUpdateThread(){
    updateThread = std::thread(&CameraCapture::ThreadUpdateFunction, this);
}

void CameraCapture::StopUpdateThread(){
    if(updateThread.joinable()){
        updateThread.join();
        printf("Update thread stopped.\n");
    }
}

//Will terminate automatically when the program exits.
void CameraCapture::ThreadUpdateFunction(){
    while(FaceOffGlobals::ThreadsShouldStop == false){
        Update();
    }
}






