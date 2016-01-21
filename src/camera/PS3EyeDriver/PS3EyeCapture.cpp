//
//  PS3EyeCapture.cpp
//  FaceOff
//
//  Created by Jonathan Cole on 10/22/15.
//
//

#include "PS3EyeCapture.hpp"

PS3EyeCapture::PS3EyeCapture(){
    //PS3EyeDriver::GetInstance().Update();
    printf("[PS3EyeCapture] constructor\n");
}

PS3EyeCapture::~PS3EyeCapture(){
    printf("[PS3EyeCapture] destructor\n");
    free(rawData);
    if(useThreadedUpdate) PS3EyeDriver::GetInstance().StopThreadUpdate();
}

bool PS3EyeCapture::Init(const int deviceNum){
    //printf("PS3 Eye cameras connected: %i\n", PS3EyeDriver::GetInstance().GetNumCameras());
    SetDeviceIndex(deviceNum);
    
    //The requested FPS will be changed to the next-lowest valid framerate if an invalid framerate is requested.
    bool success = PS3EyeDriver::GetInstance().InitCamera(deviceNum, PS3EyeDriver::RESOLUTION_SETTING::FULL_640_480, 60);
    if(success){
        SetFrameWidth(PS3EyeDriver::GetInstance().GetHeight(deviceNum));
        SetFrameHeight(PS3EyeDriver::GetInstance().GetWidth(deviceNum));
        rawData = (unsigned char*)malloc(GetFrameWidth()*GetFrameHeight()*3*sizeof(unsigned char));
        //cv::Mat frame(cv::Mat(cv::Size(GetFrameWidth(), GetFrameHeight()), CV_8U));
        if(useThreadedUpdate) PS3EyeDriver::GetInstance().StartThreadUpdate();
    }
    else{
        //printf("Unsuccessful\n");
    }
    return success;
}


//Draw loop always pulls the newest completed frame (optionally: and only performs CV operations when the frame is new).
//Update thread converts and stores latest camera data into frame (slow). Only when this conversion is done should
//  the draw loop grab the data.
void PS3EyeCapture::Update(){
    //TODO: This gets called twice per frame, once per camera object.
    //We need to make sure this only gets called once across all Update calls this frame.
    if(!useThreadedUpdate) PS3EyeDriver::GetInstance().Update();
    
    //Each frame assume the frame isn't ready
    //frameIsReady = false;
    //printf("Update status: %i\n", res);
    
    //If the camera has new data, grab the data and convert it
    if(PS3EyeDriver::GetInstance().HasNewFrame(GetDeviceIndex())){ //&& frameIsReady == false here //(Mutex lock kind of)
        //printf("New frame\n");
        //Pull latest converted frame from PS3EyeDriver
        //Store raw data into buffer
        
        
        PS3EyeDriver::GetInstance().ConvertRawData(GetDeviceIndex(), rawData);
        //rawData = PS3EyeDriver::GetInstance().GetRawFrame(GetDeviceIndex());
        
        //cv::Mat* frame(GetFrameWidth(), GetFrameHeight(), CV_8U, rawData); // does not copy
        
        //frame = cv::Mat(GetFrameWidth(), GetFrameHeight(), CV_8UC3, rawData);
        frame.data = rawData;
        
        //Swap the buffers
        frameIsReady = true;
    }
}

const bool PS3EyeCapture::FrameIsReady(){
    return frameIsReady;
}