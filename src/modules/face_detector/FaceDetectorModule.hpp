//
//  FaceDetectorModule.hpp
//  FaceOff
//
//  Created by Jonathan Cole on 11/3/15.
//
//

#ifndef FaceDetectorModule_hpp
#define FaceDetectorModule_hpp

#include "opencv2/opencv.hpp"
#include "opencv2/objdetect.hpp"

#include "ModuleCommon.hpp"
#include "UsesGUI.hpp"

using namespace std;
using namespace cv;

class FaceDetectorModule : ModuleCommon, UsesGUI {
public:
    FaceDetectorModule();
    ~FaceDetectorModule();
    
    void ProcessFrame(cv::InputArray in, cv::OutputArray out);
    
    void SetupGUIVariables() override;
    
private:
    CascadeClassifier face_cascade;
    CascadeClassifier eyes_cascade;
    
    float imageScale = 0.125;
    
};

#endif /* FaceDetectorModule_hpp */
