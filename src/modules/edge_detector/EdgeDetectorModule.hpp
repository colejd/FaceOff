//
//  EdgeDetectorModule.hpp
//  FaceOff
//
//  Created by Jonathan Cole on 10/29/15.
//
//

#ifndef EdgeDetectorModule_hpp
#define EdgeDetectorModule_hpp

#include <stdio.h>
#include <string>

#include "opencv2/opencv.hpp"
#include <opencv2/core/ocl.hpp>
#include <opencv2/ximgproc.hpp>

#include "UsesGUI.hpp"
#include "ConfigHandler.hpp"
#include "ParallelContourDetector.hpp"

using namespace std;
using namespace cv;

class EdgeDetectorModule : UsesGUI {
public:
    EdgeDetectorModule();
    ~EdgeDetectorModule();
    
    cv::Mat ProcessFrame(cv::Mat& frame);
    cv::Mat CompositeImages(cv::Mat& result, cv::Mat& base);
    
    enum ChannelType{
        GRAYSCALE,
        HUE //huehuehuehue
    };
    std::vector<string> channelTypeVec {"Grayscale", "Hue"};
    
    
private:
    
    int currentChannelType = ChannelType::GRAYSCALE;
    //The final image that will be shown
    Mat finalMat;
    
    void SetupGUIVariables() override;
    
    bool drawEdges = true;
    bool drawCoolEdges = false;
    int cannyThresholdLow = 30; //0
    int cannyThresholdHigh = 50; //50
    
    bool useContours = false;
    
    bool doErosionDilution = false;
    int erosionIterations = 1;
    int dilutionIterations = 1;
    
    int contourSubdivisions = 4;
    
    cv::Mat edges, contours, rawFrame;
    
    int lineThickness = 2;
    
    bool showEdgesOnly = true;
    
    Color lineColor = Color(255, 255, 0);
    
    String path = "/Users/jonathancole/Dev/Projects/Work/FaceOff/xcode/build/Release/model.yml.gz";
    Ptr<cv::ximgproc::StructuredEdgeDetection> pDollar = cv::ximgproc::createStructuredEdgeDetection(path);
    
    static cv::Scalar ColorToScalar(const Color& c){
        return cv::Scalar(c[2] * 255.0f, c[1] * 255.0f, c[0] * 255.0f);
    }
};

#endif /* EdgeDetectorModule_hpp */
