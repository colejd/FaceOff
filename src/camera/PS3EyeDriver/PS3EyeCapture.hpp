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
#include "PS3EyeDriver.hpp"

class PS3EyeCapture : public CaptureBase {
public:
    PS3EyeCapture();
    ~PS3EyeCapture();
    bool Init(const int deviceNum) override;
    void Update() override;
    const bool FrameIsReady() override;
    
    void SwapBuffers();
    
private:
    uint8_t *rawData;
    
};

#endif /* PS3EyeCapture_hpp */
