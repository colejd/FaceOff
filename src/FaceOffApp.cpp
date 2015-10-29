#include "FaceOffApp.h"

/** 
 Runs before the rest of the application starts.
 You should only alter Cinder settings from here. 
 */
void prepareSettings( App::Settings *settings )
{
    //settings->setHighDensityDisplayEnabled();
    settings->setWindowPos(0, 0);
    settings->setFrameRate(120);
    //Prevent the computer from dimming the display or sleeping
    settings->setPowerManagementEnabled(false);
}

/**
 Runs at the start of the program.
 */
void FaceOffApp::setup(){
    
    //Print debug info
    #ifdef DEBUG
        std::cout << "Debug build\n";
        //std::cout << cv::getBuildInformation();
    #endif
    
    static int WINDOW_WIDTH = ConfigHandler::GetInstance().config.lookup("DEFAULT_WINDOW_WIDTH");
    static int WINDOW_HEIGHT = ConfigHandler::GetInstance().config.lookup("DEFAULT_WINDOW_HEIGHT");
    
    //Set up app window
    setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    //Set up OpenCV
    cv::ocl::setUseOpenCL(false);
    
    //Set up camera device
    capture1 = new CameraCapture();
    capture1->Init(0, CameraCapture::DEVICE_TYPE::PS3EYE);
    //Quit if the capture doesn't take
    if(capture1->IsInitialized()){
        capture1->StartUpdateThread();
    }
    else{
        printf("There was a problem initializing the camera capture.\n");
        exit(EXIT_FAILURE);
    }
    
    
    capture2 = new CameraCapture();
    capture2->Init(1, CameraCapture::DEVICE_TYPE::PS3EYE);
    //Quit if the capture doesn't take
    if(capture2->IsInitialized()){
        capture2->StartUpdateThread();
    }
    else{
        printf("There was a problem initializing the camera capture.\n");
        exit(EXIT_FAILURE);
    }
     
    
    
    //frame.create(cv::Size(capture.GetWidth(), capture.GetHeight()), CV_8UC3);
    
    setWindowSize(capture1->GetWidth() * 2, capture1->GetHeight());
    
    //edges = cv::Mat(cv::Size(capture.GetWidth(), capture.GetHeight()), CV_8U);
    
    SetupGUIVariables();
    
    static bool FULLSCREEN_ON_LAUNCH = ConfigHandler::GetInstance().config.lookup("FULLSCREEN_ON_LAUNCH");
    setFullScreen(FULLSCREEN_ON_LAUNCH);
    printf("FaceOff Init finished.\n");
    
}

void FaceOffApp::SetupGUIVariables(){
    GetGUI().GetParams()->addParam("Edge Detection", &drawEdges).keyIncr("e");
    GetGUI().GetParams()->addParam("Low Threshold", &cannyThresholdLow).max(255).min(0);
    GetGUI().GetParams()->addParam("High Threshold", &cannyThresholdHigh).max(255).min(0);
}


void FaceOffApp::update(){
    
    //Grab a new frame from the camera
    
    //Capture is very slow sometimes. Why?
    //capture.Update();
    
    if(capture1->IsInitialized()){
        if(capture1->FrameIsReady()){
            //frame = capture.GetLatestFrame();
            
            cv::Mat rawFrame(capture1->GetWidth(), capture1->GetHeight(), CV_8UC3, capture1->GetLatestFrame()->data);
            cv::Mat edges;
            //printf("Data1: %i\n", rawFrame.data[100]);
            //printf("Width: %i, Height: %i\n", rawFrame.cols, rawFrame.rows);
            
            //imshow("Raw Frame", rawFrame);
            
            //Do computer vision stuff
            if(!rawFrame.empty()){
                if(drawEdges){
                    //Downsample
                    //cv::resize(frame, frame, cv::Size(), 0.5, 0.5, INTER_NEAREST);
                    //Convert to grayscale
                    cvtColor(rawFrame, edges, COLOR_BGR2GRAY);
                    //Blur the result to reduce noise
                    GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5);
                    //Run Canny detection on the blurred image
                    Canny(edges, edges, cannyThresholdLow, cannyThresholdHigh, 3); //0, 30, 3
                    //cv::resize(edges, edges, cv::Size(), 2.0, 2.0, INTER_NEAREST);
                    
                    //Convert the grayscale edges mat back into BGR
                    cvtColor(edges, finalImageLeft, cv::COLOR_GRAY2BGR);
                }
                else{
                    rawFrame.copyTo(finalImageLeft);
                    //cvtColor(rawFrame, finalImage, cv::COLOR_BGR2BGR);
                }
                
                //putText(finalImage, format("OpenCL: %s", ocl::useOpenCL() ? "ON" : "OFF"), cv::Point(0, 50), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 255, 255), 2);
                
                //Send final image data to Cinder
                //frame_bgra = finalImage.data;
                //finalImageData = Surface(frame_bgra, capture.GetWidth(), capture.GetHeight(), capture.GetWidth()*4, SurfaceChannelOrder::BGRA);
            }
            //Directly draw the data from the frame
            else{
                printf("Frame is empty\n");
                //finalImageData = Surface(rawFrame.data, capture.GetWidth(), capture.GetHeight(), capture.GetWidth()*3, SurfaceChannelOrder::RGB);
                //finalTexture = gl::Texture::create( finalImageData );
            }
            
            capture1->MarkFrameUsed();
            rawFrame.release();
        }
    }
    
    
    if(capture2->IsInitialized()){
        if(capture2->FrameIsReady()){
            //frame = capture.GetLatestFrame();
            
            cv::Mat rawFrame2(capture2->GetWidth(), capture2->GetHeight(), CV_8UC3, capture2->GetLatestFrame()->data);
            cv::Mat edges;
            //printf("Data2: %i\n", rawFrame2.data[100]);
            //printf("Width: %i, Height: %i\n", rawFrame2.cols, rawFrame2.rows);
            
            //Do computer vision stuff
            if(!rawFrame2.empty()){
                if(drawEdges){
                    //Downsample
                    //cv::resize(frame, frame, cv::Size(), 0.5, 0.5, INTER_NEAREST);
                    //Convert to grayscale
                    cvtColor(rawFrame2, edges, COLOR_BGR2GRAY);
                    //Blur the result to reduce noise
                    GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5);
                    //Run Canny detection on the blurred image
                    Canny(edges, edges, cannyThresholdLow, cannyThresholdHigh, 3); //0, 30, 3
                    //cv::resize(edges, edges, cv::Size(), 2.0, 2.0, INTER_NEAREST);
                    
                    //Convert the grayscale edges mat back into BGR
                    cvtColor(edges, finalImageRight, cv::COLOR_GRAY2BGR);
                }
                else{
                    rawFrame2.copyTo(finalImageRight);
                    //cvtColor(rawFrame, finalImage, cv::COLOR_BGR2BGR);
                }
                
                //putText(finalImage, format("OpenCL: %s", ocl::useOpenCL() ? "ON" : "OFF"), cv::Point(0, 50), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 255, 255), 2);
                
                //Send final image data to Cinder
                //frame_bgra = finalImage.data;
                //finalImageData = Surface(frame_bgra, capture.GetWidth(), capture.GetHeight(), capture.GetWidth()*4, SurfaceChannelOrder::BGRA);
            }
            //Directly draw the data from the frame
            else{
                printf("Frame is empty\n");
                //finalImageData = Surface(rawFrame.data, capture.GetWidth(), capture.GetHeight(), capture.GetWidth()*3, SurfaceChannelOrder::RGB);
                //finalTexture = gl::Texture::create( finalImageData );
            }
            
            capture2->MarkFrameUsed();
            rawFrame2.release();
        }
    }
    

}


void FaceOffApp::draw(){
    gl::clear( Color( 0, 0, 0 ) );
    
    //For drawing the image and keeping aspect ratio
    //https://gist.github.com/jkosoy/3895744
    
    //Draw the final image
    //gl::setMatricesWindow( capture1->GetWidth() * 2, capture1->GetHeight() );
    //if( finalTexture ) {
        //gl::pushViewMatrix();
        gl::draw(GetTextureFromMat(finalImageLeft), Rectf(0, 0, getWindowSize().x/2, getWindowSize().y) );
        gl::draw(GetTextureFromMat(finalImageRight), Rectf(getWindowSize().x/2, 0, getWindowSize().x, getWindowSize().y));
        //gl::popViewMatrix();
    //}
    
    //Draw the FPS counter
    char fpsString[50];
    sprintf(fpsString, "FPS: %.2f", getAverageFps());
    
    Font mFont = Font( "Courier", 20 );
    gl::TextureRef mTextTexture;
    vec2 mSize = vec2( 100, 100 );
    TextBox tbox = TextBox().alignment( TextBox::CENTER ).font( mFont ).size( ivec2( mSize.x, TextBox::GROW ) );
    tbox.text(fpsString);
    tbox.setColor( Color( 1.0f, 0.65f, 0.35f ) );
    tbox.setBackgroundColor( ColorA( 0, 0, 0, 1 ) );
    mTextTexture = gl::Texture2d::create( tbox.render() );
    if( mTextTexture ){
        gl::draw( mTextTexture );
    }
    
    //Draw the GUI
    GUIHandler::GetInstance().Draw();
    
}

cinder::gl::Texture2dRef FaceOffApp::GetTextureFromMat(cv::Mat& mat){
    Surface8u imageData(mat.data, mat.cols, mat.rows, mat.cols*3, SurfaceChannelOrder::BGR);
    return gl::Texture::create( imageData );
}

void FaceOffApp::mouseDown( MouseEvent event ){
}

void FaceOffApp::keyDown( KeyEvent event )
{
    if( event.getChar() == 'f' ) {
        // Toggle full screen when the user presses the 'f' key.
        setFullScreen( ! isFullScreen() );
    }
    else if( event.getCode() == KeyEvent::KEY_SPACE ) {
    }
    else if( event.getCode() == KeyEvent::KEY_ESCAPE ) {
        // Exit full screen, or quit the application, when the user presses the ESC key.
        if( isFullScreen() )
            setFullScreen( false );
        else
            QuitApp();
    }
    else if( event.getCode() == KeyEvent::KEY_w ){
        exit(EXIT_SUCCESS);
    }
}

void FaceOffApp::QuitApp(){
    FaceOffGlobals::ThreadsShouldStop = true;
    quit();
}

CINDER_APP( FaceOffApp, RendererGl, prepareSettings )
