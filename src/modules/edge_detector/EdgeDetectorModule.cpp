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
    rawFrame.release();
    edges.release();
    contours.release();
}

void EdgeDetectorModule::SetupGUIVariables(){
    GetGUI().AddWindow("Edge Detector", ivec2(200, 200));
    params::InterfaceGlRef edgesWindow = GetGUI().GetWindow("Edge Detector");
    edgesWindow->setPosition(ivec2(210, 0));
    edgesWindow->addParam("Edge Detection", &drawEdges).keyIncr("e");
    edgesWindow->addParam("Use Contours", &useContours).keyIncr("c");
    edgesWindow->addParam("Low Threshold", &cannyThresholdLow).min(0).max(255);
    edgesWindow->addParam("High Threshold", &cannyThresholdHigh).min(0).max(255);
    edgesWindow->addParam("Contour Subdivisions", &contourSubdivisions).min(1).max(16);
    edgesWindow->addParam("Draw cool edges", &drawCoolEdges).keyIncr("d");
    edgesWindow->addParam("Contour Thickness", &lineThickness).min(-1).max(8);
    edgesWindow->addParam("Erosion/Dilution", &doErosionDilution);
    edgesWindow->addParam("Erosion Iterations", &erosionIterations).min(0).max(6);
    edgesWindow->addParam("Dilution Iterations", &dilutionIterations).min(0).max(6);
    edgesWindow->addParam("Line Color", &lineColor, "", false);
    edgesWindow->addParam("Show Edges Only", &showEdgesOnly);
    
    //! Adds enumerated parameter. The value corresponds to the indices of \a enumNames.
    //Options<int> addParam( const std::string &name, const std::vector<std::string> &enumNames, int *param, bool readOnly = false );
    
    edgesWindow->addParam("Channel Type", channelTypeVec, &currentChannelType);
}

cv::Mat EdgeDetectorModule::ProcessFrame(cv::Mat& frame){
    rawFrame = frame; //Shallow copy
    
    //printf("Data1: %i\n", rawFrame.data[100]);
    //printf("Width: %i, Height: %i\n", rawFrame.cols, rawFrame.rows);
    
    //imshow("Raw Frame", rawFrame);
    
    //Do computer vision stuff
    
    if(!rawFrame.empty()){
        
        if(currentChannelType == ChannelType::GRAYSCALE){
            cvtColor(rawFrame, edges, COLOR_BGR2GRAY);
        }
        else{
            std::vector<cv::Mat> channels;
            cv::Mat hsv;
            cv::cvtColor( rawFrame, hsv, CV_RGB2HSV );
            cv::split(hsv, channels);
            edges = channels[0];
            hsv.release();
        }
        
        if(drawEdges){
            
            //Perform erosion and dilution if requested
            if(doErosionDilution){
                erode(edges, edges, Mat(), cv::Point(-1,-1), erosionIterations, BORDER_CONSTANT, morphologyDefaultBorderValue());
                dilate(edges, edges, Mat(), cv::Point(-1,-1), dilutionIterations, BORDER_CONSTANT, morphologyDefaultBorderValue());
            }
            
            if(!drawCoolEdges){
                //Downsample
                //cv::resize(frame, frame, cv::Size(), 0.5, 0.5, INTER_NEAREST);
                
                //Convert to grayscale
            
                //Blur the result to reduce noise
                GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5);
                //Run Canny detection on the blurred image
                Canny(edges, edges, cannyThresholdLow, cannyThresholdHigh, 3); //0, 30, 3
                
                //cv::resize(edges, edges, cv::Size(), 2.0, 2.0, INTER_NEAREST);
                
                if(useContours) cv::parallel_for_(cv::Range(0, contourSubdivisions), ParallelContourDetector(edges, edges, contourSubdivisions, lineThickness));
            
                if(showEdgesOnly){
                    cvtColor(edges, finalMat, cv::COLOR_GRAY2BGR);
                    finalMat.setTo(ColorToScalar(lineColor), edges);
                }
                else{
                    rawFrame.copyTo(finalMat);
                    finalMat.setTo(ColorToScalar(lineColor), edges);
                    //edges.copyTo(finalMat, edges);
                }
            }
            else{
                //http://docs.opencv.org/master/d0/da5/tutorial_ximgproc_prediction.html
                using namespace cv;
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
                cvtColor(result, finalMat, cv::COLOR_GRAY2BGR);
            }
        
        }
        else{
            rawFrame.copyTo(finalMat);
            //cvtColor(rawFrame, finalImage, cv::COLOR_BGR2BGR);
        }
    }
    //Directly draw the data from the frame
    else{
        printf("[EdgeDetectorModule] Frame is empty\n");
    }
    
    return finalMat;

}