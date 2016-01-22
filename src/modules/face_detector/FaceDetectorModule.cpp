//
//  FaceDetectorModule.cpp
//  FaceOff
//
//  Created by Jonathan Cole on 11/3/15.
//
//

#include "FaceDetectorModule.hpp"

FaceDetectorModule::FaceDetectorModule(){
    face_cascade.load("data/haarcascade_frontalface_alt.xml") ;
    eyes_cascade.load("data/haarcascade_eye.xml");
    
    //SetupGUIVariables();
}

FaceDetectorModule::~FaceDetectorModule(){
    
}

void FaceDetectorModule::ProcessFrame(cv::InputArray in, cv::OutputArray out){
    if(IsEnabled()){
        
        float scale = imageScale;
        float unscale = 1.0/scale;
        
        cv::UMat latestStep;
        in.copyTo(latestStep);
        
        cv::resize(latestStep, latestStep, cv::Size(), scale, scale, INTER_NEAREST);
        cvtColor( latestStep, latestStep, CV_BGR2GRAY );
        equalizeHist( latestStep, latestStep );
        
        cv::UMat outCopy;
        //outCopy = cv::Mat(in.cols(), in.rows(), in.type());
        out.copyTo(outCopy);
        
        
        std::vector<cv::Rect> faces;
        
        //-- Detect faces
        face_cascade.detectMultiScale( latestStep, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(30, 30) );
        
        //http://docs.opencv.org/3.0.0/db/d28/tutorial_cascade_classifier.html
        
        for( size_t i = 0; i < faces.size(); i++ )
        {
            cv::Point center( (faces[i].x + faces[i].width*0.5) * unscale, (faces[i].y + faces[i].height*0.5) * unscale);
            ellipse( outCopy, center, cv::Size( (faces[i].width*0.5) * unscale, (faces[i].height*0.5) * unscale), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
            
            
            /*
         
            cv::UMat faceROI = latestStep( faces[i] );
            std::vector<cv::Rect> eyes;
            
            //-- In each face, detect eyes
            eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, cv::Size(30, 30) );
            for( size_t j = 0; j < eyes.size(); j++ )
            {
                cv::Point eye_center( (faces[i].x + eyes[j].x + eyes[j].width/2) * unscale, (faces[i].y + eyes[j].y + eyes[j].height/2) * unscale );
                int radius = cvRound( (eyes[j].width + eyes[j].height) * 0.25 * unscale );
                circle( outCopy, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
            }
             */
         
            
        }
        
        outCopy.copyTo(out);
    
        
    }
    else{
        //in.copyTo(out);
    }
}

void FaceDetectorModule::SetupGUIVariables(){
    GetGUI().AddWindow("Face Detector", ivec2(300, 195));
    params::InterfaceGlRef facesWindow = GetGUI().GetWindow("Face Detector");
    facesWindow->setPosition(ivec2(515, 5));
    
    //Settings
    facesWindow->addParam("Module Enabled", &enabled).keyIncr("e");
    facesWindow->addSeparator("All");
    facesWindow->addParam("Scale", &imageScale).min(0.01).max(1.00);
    ShowHelpMarker("Higher values give more accurate results, but run much slower.");
    
    
}

void FaceDetectorModule::DrawGUI(){
    if(showGUI){
        ui::Begin("Face Detector", &showGUI, ImGuiWindowFlags_AlwaysAutoResize);
        
        ui::Checkbox("Enabled", &enabled);
        ui::SliderFloat("Scale", &imageScale, 0.01, 1.00);
        
        ui::End();
    }
}