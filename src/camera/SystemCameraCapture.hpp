//
//  SystemCameraCapture.hpp
//  FaceOff
//
//  Created by Jonathan Cole on 10/22/15.
//
//

#ifndef SystemCameraCapture_hpp
#define SystemCameraCapture_hpp

#include "CaptureBase.h"

using namespace cv;

/**
 Capture implementation for OS-detected cameras.
 */
class SystemCameraCapture : public CaptureBase {
    
public:
    bool Init(const int deviceNum) override;
    void Update() override;
    const bool FrameIsReady() override;
    
private:
    VideoCapture cap;
    
};

#endif /* SystemCameraCapture_hpp */
