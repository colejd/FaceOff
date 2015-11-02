//
//  EdgeDetectorModule.cpp
//  FaceOff
//
//  Created by Jonathan Cole on 10/29/15.
//
//

#include "EdgeDetectorModule.hpp"

EdgeDetectorModule::EdgeDetectorModule(){
    SetupGUIVariables();
}

EdgeDetectorModule::~EdgeDetectorModule(){
}

void EdgeDetectorModule::Enable(){
    enabled = true;
}

void EdgeDetectorModule::Disable(){
    enabled = false;
}

const bool EdgeDetectorModule::IsEnabled(){
    return enabled;
}

void EdgeDetectorModule::SetupGUIVariables(){
    GetGUI().AddWindow("Edge Detector", ivec2(300, 195));
    params::InterfaceGlRef edgesWindow = GetGUI().GetWindow("Edge Detector");
    edgesWindow->setPosition(ivec2(210, 5));
    
    //Settings
    edgesWindow->addParam("Module Enabled", &enabled).keyIncr("e");
    edgesWindow->addSeparator("All");
    edgesWindow->addParam("Low Threshold", &cannyThresholdLow).min(0).max(255);
    edgesWindow->addParam("High Threshold", &cannyThresholdHigh).min(0).max(255);
    edgesWindow->addParam("Line Color", &lineColor, "", false);
    edgesWindow->addParam("Show Edges Only", &showEdgesOnly);
    edgesWindow->addParam("Channel Type", channelTypeVec, &currentChannelType);
    
    //Contour settings
    edgesWindow->addParam("Use Contours", &useContours).keyIncr("c").group("Contour Settings");
    edgesWindow->addParam("Contour Subdivisions", &contourSubdivisions).min(1).max(16).group("Contour Settings");
    edgesWindow->addParam("Contour Thickness", &lineThickness).min(-1).max(8).group("Contour Settings");
    
    //Image tuning
    edgesWindow->addParam("Blur Type", blurTypeVec, &currentBlurType);
    edgesWindow->addParam("Erosion/Dilution", &doErosionDilution).group("Image Tuning");
    edgesWindow->addParam("Erosion Iterations", &erosionIterations).min(0).max(6).group("Image Tuning");
    edgesWindow->addParam("Dilution Iterations", &dilutionIterations).min(0).max(6).group("Image Tuning");
    
    //Misc.
    edgesWindow->addParam("Draw cool edges", &drawCoolEdges).keyIncr("d").group("Misc.");
    edgesWindow->addParam("Sigma S", &sigma_s).group("Misc.").min(0).max(32);
    edgesWindow->addParam("Sigma R", &sigma_r).group("Misc.").min(0).max(1);
    
    
}

//Pass in input and output frames
cv::Mat EdgeDetectorModule::ProcessFrame(cv::Mat& frame){
    cv::Mat edges, contours, rawFrame;
    cv::Mat finalFrame;
    
    frame.copyTo(rawFrame);
    rawFrame = frame; //Shallow copy
    
    if(!rawFrame.empty() && IsEnabled()){
        rawFrame.copyTo(finalFrame);
        
        //Prepare edges for use with Canny by condensing it to one channel
        if(currentChannelType == ChannelType::GRAYSCALE){ //Channel is grayscale
            cvtColor(rawFrame, edges, COLOR_BGR2GRAY);
        }
        else if(currentChannelType == ChannelType::HUE){ //Channel is hue channel of original image
            std::vector<cv::Mat> channels;
            cv::Mat hsv;
            cv::cvtColor( rawFrame, hsv, CV_RGB2HSV );
            cv::split(hsv, channels);
            edges = channels[0];
            hsv.release();
        }
        
        //threshold(edges, edges, 0, 255, THRESH_BINARY | THRESH_OTSU);
        
        //Do blurring operation on the source image
        BlurImage(edges, edges, currentBlurType);
        
        //fastNlMeansDenoising(edges, edges, 3, 7, 21);
        
        //Perform erosion and dilution if requested
        if(doErosionDilution){
            erode(edges, edges, Mat(), cv::Point(-1,-1), erosionIterations, BORDER_CONSTANT, morphologyDefaultBorderValue());
            dilate(edges, edges, Mat(), cv::Point(-1,-1), dilutionIterations, BORDER_CONSTANT, morphologyDefaultBorderValue());
        }
        
        //Normal edge detection
        if(!drawCoolEdges){
            //Downsample
            //cv::resize(frame, frame, cv::Size(), 0.5, 0.5, INTER_NEAREST);
            
            //Convert to grayscale
        
            //Run Canny detection on the blurred image
            
            if(currentChannelType == ChannelType::COLOR){
                std::vector<cv::Mat> channels;
                cv::split(rawFrame, channels);
                
                //Separate the three color channels and perform Canny on each
                Canny(channels[0], channels[0], cannyThresholdLow, cannyThresholdHigh, 3);
                Canny(channels[1], channels[1], cannyThresholdLow, cannyThresholdHigh, 3);
                Canny(channels[2], channels[2], cannyThresholdLow, cannyThresholdHigh, 3);
                
                //Merge the three color channels into one image
                //merge(channels, canny_output);
                bitwise_and(channels[0], channels[1], channels[1]);
                bitwise_and(channels[1], channels[2], edges);
            }
            else{
                Canny(edges, edges, cannyThresholdLow, cannyThresholdHigh, 3); //0, 30, 3
            }
            
            //cv::resize(edges, edges, cv::Size(), 2.0, 2.0, INTER_NEAREST);
            
            if(useContours) cv::parallel_for_(cv::Range(0, contourSubdivisions), ParallelContourDetector(edges, edges, contourSubdivisions, lineThickness));
        
            if(showEdgesOnly){
                cvtColor(edges, finalFrame, COLOR_GRAY2BGR);
                finalFrame.setTo(ColorToScalar(lineColor), edges);
            }
            else{
                rawFrame.copyTo(finalFrame);
                finalFrame.setTo(ColorToScalar(lineColor), edges);
                //edges.copyTo(finalMat, edges);
            }
        }
        //Structured edge detection
        else{
            //http://docs.opencv.org/master/d0/da5/tutorial_ximgproc_prediction.html
            using namespace cv::ximgproc;
            
            cv::Mat cool;
            //rawFrame.convertTo(cool, CV_32FC3);
            rawFrame.convertTo( cool, cv::DataType<float>::type, 1/255.0 );
            
            cv::Mat edges( cool.size(), cool.type() );
            
            //String path = "../../../model.yml.gz";
            pDollar->detectEdges(cool, edges);
            //std::cout << "Edges - " << "Channels: " << edges.channels() << " Element Size: " << edges.elemSize() << "\n";
            
            cv::Mat result;
            
            //imshow("Edges", edges);
            edges.convertTo(result, CV_8UC1, 255);
            //std::cout << "Result - " << "Channels: " << result.channels() << " Element Size: " << result.elemSize() << "\n";
            cvtColor(result, finalFrame, cv::COLOR_GRAY2BGR);
        }
        
    }
    //Directly draw the data from the frame
    else{
        printf("[EdgeDetectorModule] Frame is empty\n");
        return frame;
    }
    
    rawFrame.release();
    edges.release();
    contours.release();
    
    return finalFrame;

}

/**
 Applies a blurring operation to the image based on blurType. Defaults to, uh, 
 BlurType::DEFAULT, which is the regular OpenCV kernel blur function.
 */
void EdgeDetectorModule::BlurImage(cv::Mat &in, cv::Mat &out, int blurType = 0){
    switch(blurType){
        default:
        case(BlurType::DEFAULT):
            blur(in, out, cv::Size(7, 7));
            break;
        case(BlurType::GAUSSIAN):
            GaussianBlur(in, out, cv::Size(7,7), 1.5, 1.5);
            break;
        case(BlurType::ADAPTIVE_MANIFOLD):
            cv::ximgproc::amFilter(cv::noArray(), in, out, sigma_s, sigma_r);
            break;
            
            
    }
}