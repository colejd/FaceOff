#include "FaceOffApp.h"

/** 
 Runs before the rest of the application starts.
 You should only alter Cinder settings from here. 
 */
void prepareSettings( App::Settings *settings )
{
    //settings->setHighDensityDisplayEnabled();
    settings->setWindowPos(0, 0);
    settings->setFrameRate(60);
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
    capture = CameraCapture();
    //bool success = capture.Init(0, CameraCapture::DEVICE_TYPE::GENERIC);
    bool success = capture.Init(0, CameraCapture::DEVICE_TYPE::PS3EYE);
    //Quit if the capture doesn't take
    if(!success){
        printf("There was a problem initializing the camera capture.\n");
        exit(EXIT_FAILURE);
    }
    
    //frame.create(cv::Size(capture.GetWidth(), capture.GetHeight()), CV_8UC3);
    
    setWindowSize(capture.GetWidth(), capture.GetHeight());
    
    frame_bgra = new uint8_t[capture.GetWidth() * capture.GetHeight() * 4];
    memset(frame_bgra, 0, capture.GetWidth() * capture.GetHeight() * 4);
    
    edges = cv::Mat(cv::Size(capture.GetWidth(), capture.GetHeight()), CV_8U);
    
    SetupGUIVariables();
    
    static bool FULLSCREEN_ON_LAUNCH = ConfigHandler::GetInstance().config.lookup("FULLSCREEN_ON_LAUNCH");
    setFullScreen(FULLSCREEN_ON_LAUNCH);
    
    capture.StartUpdateThread();
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
    
    if(capture.FrameIsReady()){
        //frame = capture.GetLatestFrame();
        
        cv::Mat rawFrame(capture.GetWidth(), capture.GetHeight(), CV_8UC3, capture.GetLatestFrame()->data);
        
        //printf("Data: %i\n", rawFrame.data[100]);
        //printf("Width: %i, Height: %i\n", rawFrame.cols, rawFrame.rows);
        
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
                cvtColor(edges, finalImage, cv::COLOR_GRAY2BGRA);
            }
            else{
                cvtColor(rawFrame, finalImage, cv::COLOR_BGR2BGRA);
            }
            
            //putText(finalImage, format("OpenCL: %s", ocl::useOpenCL() ? "ON" : "OFF"), cv::Point(0, 50), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 255, 255), 2);
            
            //Send final image data to Cinder
            frame_bgra = finalImage.data;
            finalImageData = Surface(frame_bgra, capture.GetWidth(), capture.GetHeight(), capture.GetWidth()*4, SurfaceChannelOrder::BGRA);
            finalTexture = gl::Texture::create( finalImageData );
        }
        //Directly draw the data from the frame
        else{
            printf("Frame is empty\n");
            //finalImageData = Surface(rawFrame.data, capture.GetWidth(), capture.GetHeight(), capture.GetWidth()*3, SurfaceChannelOrder::RGB);
            //finalTexture = gl::Texture::create( finalImageData );
        }
        
        capture.MarkFrameUsed();
        rawFrame.release();
    }
}


void FaceOffApp::draw(){
    gl::clear( Color( 0, 0, 0 ) );
    
    //For drawing the image and keeping aspect ratio
    //https://gist.github.com/jkosoy/3895744
    
    //Draw the final image
    gl::setMatricesWindow( capture.GetWidth(), capture.GetHeight() );
    if( finalTexture ) {
        gl::pushViewMatrix();
        gl::draw( finalTexture );
        gl::popViewMatrix();
    }
    
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
