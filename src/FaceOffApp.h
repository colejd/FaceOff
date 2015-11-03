//
//  FaceOffApp.h
//  FaceOff
//
//  Created by Jonathan Cole on 10/21/15.
//
//

#ifndef FaceOffApp_h
#define FaceOffApp_h

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/gl/Texture.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#include "opencv2/opencv.hpp"
#include <opencv2/core/ocl.hpp>
#include <opencv2/ximgproc.hpp>

using namespace cv;

#include "performance/FPSCounter.h"
#include "gui/GUIHandler.hpp"
#include "gui/UsesGUI.hpp"
#include "config/ConfigHandler.hpp"
#include "camera/CameraCapture.hpp"

#include <memory>

#include "FaceOffGlobals.hpp"

#include "EdgeDetectorModule.hpp"

/**
 The main application. This program is designed to interpret data
 from an attached camera using OpenCV.
 */
class FaceOffApp : public App, public UsesGUI {
    
public:
    
    void setup() override;
    // Cinder will call 'keyDown' when the user presses a key on the keyboard.
    // See also: keyUp.
    void keyDown( KeyEvent event ) override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    // Cinder will call 'draw' each time the contents of the window need to be redrawn.
    void draw() override;
    
    void PrintDebugInfo();
    
    void QuitApp();
    
    void SetupGUIVariables() override;
    
    cinder::gl::Texture2dRef GetTextureFromMat(cv::Mat& mat);
    
private:
    
    //const int WINDOW_WIDTH = 640;
    //const int WINDOW_HEIGHT = 400;
    
    CameraCapture* capture1;
    CameraCapture* capture2;
    
    //The final image that will be shown
    Mat finalImageLeft;
    Mat finalImageRight;
    
    EdgeDetectorModule edgeDetector;
    
};



#endif /* FaceOffApp_h */
