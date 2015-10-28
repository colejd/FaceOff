//
//  PS3EyeDriver.hpp
//  PS3EyeDriver
//
//  Created by Jonathan Cole on 10/23/15.
//  Copyright © 2015 ProofOfConcept. All rights reserved.
//

#ifndef PS3EyeDriver_hpp
#define PS3EyeDriver_hpp

#include <stdio.h>
#include <thread>

#include <unordered_map>
#include "YUVBuffer.hpp"
#include "ps3Eye.h"

/* The classes below are exported */
#pragma GCC visibility push(default)

/**
 Entry point into PS3 Eye driver code.
 */
class PS3EyeDriver {
public:
    //==== SINGLETON STUFF ==============================================//
    static PS3EyeDriver& GetInstance()
    {
        static PS3EyeDriver instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    //==== END SINGLETON STUFF ==============================================//
    
    enum class RESOLUTION_SETTING{
        FULL_640_480, //640x480
        HALF_320_240  //320x240
    };
    
    bool InitCamera     (int index, const RESOLUTION_SETTING requestedResolution, const int requestedFPS);
    bool ReleaseCamera  (int index);
    
    /*------ Setters -----*/
    void setAutoWhiteBalance    (const int device, const bool autoWhiteBalance);
    void setAutoGain            (const int device, const bool autoGain);
    void setGain                (const int device, const float gain);
    void setSharpness           (const int device, const float sharpness);
    void setExposure            (const int device, const float exposure);
    void setBrightness          (const int device, const float brightness);
    void setContrast            (const int device, const float contrast);
    void setHue                 (const int device, const float hue); //huehuehuehuehue
    void setBlueBalance         (const int device, const float blueBalance);
    void setRedBalance          (const int device, const float redBalance);
    
    /*------ Getters -----*/
    const bool getAutoWhiteBalance  (const int device);
    const bool getAutoGain          (const int device);
    const uint8_t getGain           (const int device);
    const uint8_t getSharpness      (const int device);
    const uint8_t getExposure       (const int device);
    const uint8_t getBrightness     (const int device);
    const uint8_t getContrast       (const int device);
    const uint8_t getHue            (const int device);
    const uint8_t getBlueBalance    (const int device);
    const uint8_t getRedBalance     (const int device);
    
    const bool HasNewFrame(const int device);
    uint8_t* GetConvertedFrame(const int device);
    const uint8_t* GetRawFrame(const int device);
    
    void ConvertRawData(const int device, uint8_t *out);
    
    std::unordered_map<int, YUVBuffer> conversionBuffers;
    std::unordered_map<int, ps3eye::PS3EYECam::PS3EYERef> connectedDevices;
    const int GetRawDataLength(const int device);
    const int GetConvertedDataLength(const int device);
    
    void UpdateFrame(const int device, uint8_t* framePtr);
    
    const int GetWidth(const int device);
    const int GetHeight(const int device);
    
    const int GetNumCameras();
    
    void ThreadUpdate();
    void StartThreadUpdate();
    void StopThreadUpdate();
    
    const bool Update();
    
    bool threadShouldStop = false;
    bool threadRunning = false;
    
private:
    //==== SINGLETON STUFF ==============================================//
    PS3EyeDriver();
    ~PS3EyeDriver();
    // C++11:
    // Stop the compiler from generating copy methods for the object
    PS3EyeDriver(PS3EyeDriver const&) = delete;
    void operator=(PS3EyeDriver const&) = delete;
    //==== END SINGLETON STUFF ==============================================//
    
    //std::vector<ps3eye::PS3EYECam::PS3EYERef> devices;
    std::vector<YUVBuffer> buffers;
    
};

#pragma GCC visibility pop

#endif /* PS3EyeDriver_hpp */
