//
//  CameraCapture.cpp
//  FaceOff
//
//  Created by Jonathan Cole on 10/21/15.
//
//

#include "CameraCapture.hpp"

//std::thread updateThread;

CameraCapture::CameraCapture(){
    printf("CameraCapture constructor\n");
}

CameraCapture::~CameraCapture(){
    printf("CameraCapture destructor\n");
    StopUpdateThread();
}

/** Starts the camera capture, returning true if successful and false if not. */
bool CameraCapture::Init(const int deviceNum, const DEVICE_TYPE deviceType){
    SetDeviceType(deviceType);
    
    if(GetDeviceType() == DEVICE_TYPE::GENERIC){
        printf("Init [Generic] capture at index %i\n", deviceNum);
        currentCapture = new SystemCameraCapture();
        initialized = currentCapture->Init(deviceNum);
    }
    else if(GetDeviceType() == DEVICE_TYPE::PS3EYE){
        printf("Init [PS3EYE] capture at index %i\n", deviceNum);
        currentCapture = new PS3EyeCapture();
        initialized = currentCapture->Init(deviceNum);
    }
    else{
        initialized = false;
    }
    
    if(initialized) StartUpdateThread();
    else{
        printf("Capture init failed!\n");
    }
    
    return initialized;
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

cv::Mat& CameraCapture::GetLatestFrame(){
    return currentCapture->GetLatestFrame();
}

const bool CameraCapture::FrameIsReady(){
    if(currentCapture != nullptr)
        return currentCapture->FrameIsReady();
    else return false;
}

void CameraCapture::MarkFrameUsed(){
    currentCapture->MarkFrameUsed();
}

void CameraCapture::StartUpdateThread(){
    //updateThread = std::thread(std::bind(&CameraCapture::ThreadUpdateFunction, this));
    printf("Starting update thread\n");
    updateThread = std::thread( [this] { this->ThreadUpdateFunction(); } );
    //std::thread updateThread = std::thread( [this] { this->ThreadUpdateFunction(); } );
    //updateThread.detach();
    printf("Update thread started\n");
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


const bool CameraCapture::IsInitialized(){
    return initialized;
}





