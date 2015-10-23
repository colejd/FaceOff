//
//  PS3EyeCapture.cpp
//  FaceOff
//
//  Created by Jonathan Cole on 10/22/15.
//
//

#include "PS3EyeCapture.hpp"

bool PS3EyeCapture::Init(const int deviceNum){
    SetDeviceIndex(deviceNum);
    
    //Set Playstation 3 Eye device to use GetDeviceIndex() as its index
    
    
    //SetFrameWidth(cap.get(CV_CAP_PROP_FRAME_WIDTH));
    //SetFrameHeight(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
    
    return true;
}

void PS3EyeCapture::Update(){
    
}