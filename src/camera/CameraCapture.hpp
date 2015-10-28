//
//  CameraCapture.hpp
//  FaceOff
//
//  Created by Jonathan Cole on 10/21/15.
//
//

#ifndef CameraCapture_hpp
#define CameraCapture_hpp

#include <memory>

#include "opencv2/opencv.hpp"

#include "SystemCameraCapture.hpp"
#include "PS3EyeCapture.hpp"

#include "FaceOffGlobals.hpp"

using namespace cv;

/**
 Intended to be used for per-device camera capture. 
 In the future you'll be allowed to choose between an OpenCV capture or a Playstation 3 Eye.
 */
class CameraCapture{
public:
    
    /**
     The type of camera device looked for by this object.
     */
    enum class DEVICE_TYPE{
        /** OS-recognized camera */
        GENERIC,
        /** Uses PS3EyeDriver */
        PS3EYE
    };
    
    CameraCapture();
    ~CameraCapture();
    
    /** 
     Begins the camera capture using the camera indexed at deviceNum, starting with 0.
     Returns true if successful, and false if not.
     */
    bool Init(const int deviceNum, DEVICE_TYPE type);
    
    void SetDeviceIndex(const int index);
    const int GetDeviceIndex();
    
    void SetDeviceType(DEVICE_TYPE type);
    const DEVICE_TYPE GetDeviceType();
    
    /** Gets the width of the camera capture. */
    const int GetWidth();
    /** Gets the height of the camera capture. */
    const int GetHeight();
    
    /** Call during the update loop to pull the newest camera data. */
    void Update();
    
    const bool FrameIsReady();
    void MarkFrameUsed();
    
    //const cv::Mat& GetLatestFrame();
    std::shared_ptr<cv::Mat> GetLatestFrame();
    
    void StartUpdateThread();
    void StopUpdateThread();
    void ThreadUpdateFunction();
    //std::thread updateThread;
    
private:
    CaptureBase* currentCapture;
    DEVICE_TYPE deviceType = DEVICE_TYPE::GENERIC;
    
};

#endif /* CameraCapture_hpp */
