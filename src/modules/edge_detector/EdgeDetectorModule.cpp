//
//  EdgeDetectorModule.cpp
//  FaceOff
//
//  Created by Jonathan Cole on 10/29/15.
//
//

#include "EdgeDetectorModule.hpp"

EdgeDetectorModule::EdgeDetectorModule(){
    //SetupGUIVariables();
}

EdgeDetectorModule::~EdgeDetectorModule(){
    
}

void EdgeDetectorModule::SetupGUIVariables(){
    GetGUI().AddWindow("Edge Detector", ivec2(300, 195));
    params::InterfaceGlRef edgesWindow = GetGUI().GetWindow("Edge Detector");
    edgesWindow->setPosition(ivec2(210, 5));
    
    //Settings
    edgesWindow->addParam("Module Enabled", &enabled).keyIncr("e");
    edgesWindow->addSeparator("All");
    edgesWindow->addParam("Low Threshold", &cannyThresholdLow).min(0).max(255);
    edgesWindow->addParam("Ratio", &cannyThresholdRatio).min(2.0).max(3.0);
    edgesWindow->addParam("Line Color", &lineColor, "", false);
    edgesWindow->addParam("Show Edges Only", &showEdgesOnly);
    edgesWindow->addParam("Channel Type", channelTypeVec, &currentChannelType);
    
    //Contour settings
    edgesWindow->addParam("Use Contours", &useContours).keyIncr("c").group("Contour Settings");
    edgesWindow->addParam("Contour Subdivisions", &contourSubdivisions).min(1).max(16).group("Contour Settings");
    edgesWindow->addParam("Contour Thickness", &lineThickness).min(-1).max(8).group("Contour Settings").optionsStr("help='Set to -1 to fill all closed contours'");
    
    //Image tuning
    edgesWindow->addParam("Blur Type", blurTypeVec, &currentBlurType);
    edgesWindow->addParam("Erosion/Dilution", &doErosionDilution).group("Image Tuning");
    edgesWindow->addParam("Erosion Iterations", &erosionIterations).min(0).max(6).group("Image Tuning");
    edgesWindow->addParam("Dilution Iterations", &dilutionIterations).min(0).max(6).group("Image Tuning");
    
    //Misc.
    edgesWindow->addParam("Structured Edge Forests", &doStructuredEdgeForests).keyIncr("d").group("Misc.");
    edgesWindow->addParam("Sigma S", &sigma_s).group("Misc.").min(0).max(32);
    edgesWindow->addParam("Sigma R", &sigma_r).group("Misc.").min(0).max(1);
    edgesWindow->addParam("Sigma Color", &sigmaColor).group("Misc.").min(0).max(200);
    edgesWindow->addParam("Sigma Spatial", &sigmaSpatial).group("Misc.").min(0).max(200);
    edgesWindow->addParam("Edges Gamma", &edgesGamma).group("Misc.").min(0).max(300);
    
}

void EdgeDetectorModule::DrawGUI(){
    if(showGUI){
        ui::Begin("Edge Detector", &showGUI, ImGuiWindowFlags_AlwaysAutoResize);
        
        ui::Checkbox("Enabled", &enabled);
        ImGui::Separator();
        //if(enabled) ui::PushStyleVar(ImGuiCol_Text, ImColor::HSV(0, 0, 0.5));
        
        if(!enabled) ui::PushStyleVar(ImGuiStyleVar_Alpha, 0.2); //Push global disabled style
        
        ui::SliderInt("Low Threshold", &cannyThresholdLow, 0, 255);
            ShowHelpMarker("Set lower to look for more edges.");
        ui::SliderFloat("Ratio", &cannyThresholdRatio, 2.0, 3.0);
            ShowHelpMarker("Fine tune edge results.");
        ui::ColorEdit3("Line Color", &lineColor[0]);
        ui::Checkbox("Edges Only", &showEdgesOnly);
            ShowHelpMarker("Show just the edges.");
        ui::Combo("Channel Type", &currentChannelType, channelTypeVec);
        
        //Contour settings
        ui::Spacing();
        ui::Text("Contour Settings");
        ui::Separator();
        ui::Checkbox("Use Contours", &useContours);
            ShowHelpMarker("Use contour detection to filter out short lines and noise in the edge data.");
        if(!useContours) ui::PushStyleVar(ImGuiStyleVar_Alpha, 0.2); //Push disabled style
        ui::SliderInt("Subdivisions", &contourSubdivisions, 1, 16);
            ShowHelpMarker("Number of chunks the image is divided into for parallel processing.");
        ui::SliderInt("Thickness", &lineThickness, -1, 8);
        if(!useContours) ui::PopStyleVar(); //Pop disabled style
        
        ui::Spacing();
        ui::Text("Quality Settings");
        ui::Separator();
        ui::Combo("Blur Type", &currentBlurType, blurTypeVec);
        ui::Checkbox("Erosion/Dilution", &doErosionDilution);
            ShowHelpMarker("Try to keep erosion and dilution at the same value.");
        if(!doErosionDilution) ui::PushStyleVar(ImGuiStyleVar_Alpha, 0.2); //Push disabled style
        ui::SliderInt("Erosion Iterations", &erosionIterations, 0, 6);
            ShowHelpMarker("Morphological open operation; makes dark spots smaller and bright spots larger.");
        ui::SliderInt("Dilution Iterations", &dilutionIterations, 0, 6);
            ShowHelpMarker("Morphologial close operation; makes dark spots larger and bright spots smaller.");
        if(!doErosionDilution) ui::PopStyleVar(); //Pop disabled style
        
        if(!enabled) ui::PopStyleVar(); //Pop global disabled style
        
        ui::End();
        
    }
}


void EdgeDetectorModule::ProcessFrame(cv::InputArray in, cv::OutputArray out){
    
    if(!in.empty() && IsEnabled()){
        
        cv::UMat latestStep;
        cv::UMat finalFrame;
        
        in.copyTo(latestStep);
        
        //Condense the source image into a single channel for use with the Canny algorithm
        CondenseImage(latestStep, latestStep, currentChannelType);
        
        //threshold(edges, edges, 0, 255, THRESH_BINARY | THRESH_OTSU);
        
        //Blur the source image to reduce noise and texture details
        cv::Mat temp;
        latestStep.copyTo(temp);
        //temp = latestStep.clone();
        BlurImage(temp, temp, currentBlurType);
        
        temp.copyTo(latestStep);
        
        //fastNlMeansDenoising(edges, edges, 3, 7, 21);
        
        //Perform erosion and dilution if requested
        if(doErosionDilution){
            erode(latestStep, latestStep, UMat(), cv::Point(-1,-1), erosionIterations, BORDER_CONSTANT, morphologyDefaultBorderValue());
            dilate(latestStep, latestStep, UMat(), cv::Point(-1,-1), dilutionIterations, BORDER_CONSTANT, morphologyDefaultBorderValue());
        }
        
        //Normal edge detection
        if(!doStructuredEdgeForests){
            //Downsample
            //cv::resize(frame, frame, cv::Size(), 0.5, 0.5, INTER_NEAREST);
        
            //Canny step--------------------------------------
            //If the image has more than one color channel, then it wasn't condensed.
            //Divide it up and run Canny on each channel.
            //TODO: This should probably be run without any blurring beforehand.
            if(latestStep.channels() > 1){
                std::vector<cv::Mat> channels;
                cv::split(latestStep, channels);
                
                //Separate the three color channels and perform Canny on each
                Canny(channels[0], channels[0], cannyThresholdLow, cannyThresholdLow * cannyThresholdRatio, 3);
                Canny(channels[1], channels[1], cannyThresholdLow, cannyThresholdLow * cannyThresholdRatio, 3);
                Canny(channels[2], channels[2], cannyThresholdLow, cannyThresholdLow * cannyThresholdRatio, 3);
                
                //Merge the three color channels into one image
                //merge(channels, canny_output);
                bitwise_and(channels[0], channels[1], channels[1]);
                bitwise_and(channels[1], channels[2], latestStep);
            }
            else{
                Canny(latestStep, latestStep, cannyThresholdLow, cannyThresholdLow * cannyThresholdRatio, 3); //0, 30, 3
            }
            
            //cv::resize(edges, edges, cv::Size(), 2.0, 2.0, INTER_NEAREST);
            
            //Contour step------------------------------------
            if(useContours){
                cv::Mat contourOutput;
                latestStep.copyTo(contourOutput);
                ParallelContourDetector::DetectContoursParallel(latestStep, contourOutput, contourSubdivisions, lineThickness);
                contourOutput.copyTo(latestStep);
                //ParallelContourDetector::DetectContours(latestStep, latestStep, lineThickness);
            }
        
            //Output step-------------------------------------
            if(showEdgesOnly){
                cvtColor(latestStep, finalFrame, COLOR_GRAY2BGR);
                finalFrame.setTo(ColorToScalar(lineColor), latestStep);
            }
            else{
                in.copyTo(finalFrame);
                finalFrame.setTo(ColorToScalar(lineColor), latestStep);
            }
        }
        //Structured edge detection
        else{
            //http://docs.opencv.org/master/d0/da5/tutorial_ximgproc_prediction.html
            using namespace cv::ximgproc;
            
            cv::Mat cool;
            in.copyTo(cool);
            //rawFrame.convertTo(cool, CV_32FC3);
            cool.convertTo( cool, cv::DataType<float>::type, 1/255.0 );
            
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
        
        //Copy the result of all operations to the output frame.
        finalFrame.copyTo(out);
        
        latestStep.release();
        finalFrame.release();
        
    }
    //Directly draw the data from the frame
    else{
        if(in.empty()) printf("[EdgeDetectorModule] Frame is empty\n");
        //BlurImage(in, out, currentBlurType);
        //filterStylize(in, out);
        in.copyTo(out);
    }

}

/**
 Condenses the input image into one channel based on the ChannelType specified.
 */
void EdgeDetectorModule::CondenseImage(cv::InputArray in, cv::OutputArray out, int channelType = 0){
    if(channelType == ChannelType::GRAYSCALE){
        cvtColor(in, out, COLOR_BGR2GRAY);
    }
    else if(channelType == ChannelType::HUE){
        std::vector<cv::Mat> channels;
        cv::Mat hsv;
        cv::cvtColor( in, hsv, CV_RGB2HSV );
        cv::split(hsv, channels);
        channels[0].copyTo(out);
        hsv.release();
    }
    else if(channelType == ChannelType::COLOR){
        in.copyTo(out);
        //Do nothing
    }
}

/**
 Applies a blurring operation to the image based on blurType. Defaults to, uh,
 BlurType::DEFAULT, which is the regular OpenCV kernel blur function.
 */
void EdgeDetectorModule::BlurImage(cv::InputArray in, cv::OutputArray out, int blurType = 0){
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
        case(BlurType::DT_FILTER):
            dtFilter(in, in, out, sigmaColor, sigmaSpatial, cv::ximgproc::DTF_RF);
            break;
        case(BlurType::NONE):
            break;
            
    }
}

//stylizing filter
void EdgeDetectorModule::filterStylize(InputArray frame, OutputArray dst)
{
    //blur frame
    Mat filtered;
    dtFilter(frame, frame, filtered, sigmaSpatial, sigmaColor, cv::ximgproc::DTF_NC);
    
    //compute grayscale blurred frame
    Mat filteredGray;
    cvtColor(filtered, filteredGray, COLOR_BGR2GRAY);
    
    //find gradients of blurred image
    Mat gradX, gradY;
    Sobel(filteredGray, gradX, CV_32F, 1, 0, 3, 1.0/255);
    Sobel(filteredGray, gradY, CV_32F, 0, 1, 3, 1.0/255);
    
    //compute magnitude of gradient and fit it accordingly the gamma parameter
    Mat gradMagnitude;
    magnitude(gradX, gradY, gradMagnitude);
    cv::pow(gradMagnitude, edgesGamma/100.0, gradMagnitude);
    
    //multiply a blurred frame to the value inversely proportional to the magnitude
    Mat multiplier = 1.0/(1.0 + gradMagnitude);
    cvtColor(multiplier, multiplier, COLOR_GRAY2BGR);
    multiply(filtered, multiplier, dst, 1, dst.type());
}
