//
//  PS3EyeCapture.hpp
//  FaceOff
//
//  Created by Jonathan Cole on 10/22/15.
//
//

#ifndef PS3EyeCapture_hpp
#define PS3EyeCapture_hpp

#include "CaptureBase.h"

class PS3EyeCapture : CaptureBase {
public:
    bool Init(const int deviceNum) override;
    void Update() override;
    const bool FrameIsReady() override;
    
private:
    
};

#endif /* PS3EyeCapture_hpp */
