#include "FaceOffApp.h"

/** 
 Runs before the rest of the application starts.
 You should only alter Cinder settings from here. 
 */
void prepareSettings( App::Settings *settings )
{
    //settings->setHighDensityDisplayEnabled();
    settings->setWindowPos(0, 0);
    settings->setFrameRate(240);
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
    
    static int WINDOW_WIDTH = ConfigHandler::GetConfig().lookup("DEFAULT_WINDOW_WIDTH");
    static int WINDOW_HEIGHT = ConfigHandler::GetConfig().lookup("DEFAULT_WINDOW_HEIGHT");
    
    //Set up app window
    setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    //Set up OpenCV
    cv::ocl::setUseOpenCL(false);
    
    //Set up camera device
    capture1 = new CameraCapture();
    capture1->Init(0, CameraCapture::DEVICE_TYPE::GENERIC);
    //Quit if the capture doesn't take
    if(!capture1->IsInitialized()){
        printf("Camera is not initialized.\n");
        //exit(EXIT_FAILURE);
    }
    
    
    capture2 = new CameraCapture();
    capture2->Init(1, CameraCapture::DEVICE_TYPE::PS3EYE);
    //Quit if the capture doesn't take
    if(!capture2->IsInitialized()){
        printf("Camera is not initialized.\n");
        //exit(EXIT_FAILURE);
    }
    
    //sleep(2);
    
    
    edgeDetector = EdgeDetectorModule();
    
    setWindowSize(capture1->GetWidth() * 2, capture1->GetHeight());
    
    SetupGUIVariables();
    
    static bool FULLSCREEN_ON_LAUNCH = ConfigHandler::GetConfig().lookup("FULLSCREEN_ON_LAUNCH");
    setFullScreen(FULLSCREEN_ON_LAUNCH);
    printf("FaceOff Init finished.\n");
    
}

void FaceOffApp::SetupGUIVariables(){
    
}


void FaceOffApp::update(){
    
    //Grab a new frame from the camera
    
    if(capture1->IsInitialized()){
        if(capture1->FrameIsReady()){
            //Explicitly make a new Mat with the properties of the capture and the data of the capture's latest frame.
            //We do this because capture->GetLatestFrame returns a Mat with an invalid header but correct data.
            cv::Mat rawFrame(capture1->GetWidth(), capture1->GetHeight(), CV_8UC3, capture1->GetLatestFrame().data);
            cv::UMat rawGPUFrame;
            rawFrame.copyTo(rawGPUFrame);
            
            edgeDetector.ProcessFrame(rawGPUFrame, finalImageLeft);
            //finalImageLeft = edgeDetector.ProcessFrame(capture1->GetLatestFrame());
            
            //More or less a mutex unlock for capture->GetLatestFrame()
            capture1->MarkFrameUsed();
            rawFrame.release();
            rawGPUFrame.release();
        }
    }
    
    if(capture2->IsInitialized()){
        if(capture2->FrameIsReady()){
            //Explicitly make a new Mat with the properties of the capture and the data of the capture's latest frame.
            //We do this because capture->GetLatestFrame returns a Mat with an invalid header but correct data.
            cv::Mat rawFrame(capture2->GetWidth(), capture2->GetHeight(), CV_8UC3, capture2->GetLatestFrame().data);
            
            edgeDetector.ProcessFrame(rawFrame, finalImageRight);
            //finalImageRight = edgeDetector.ProcessFrame(capture2->GetLatestFrame());
            
            //More or less a mutex unlock for capture->GetLatestFrame()
            capture2->MarkFrameUsed();
            rawFrame.release();
        }
    }
    

}


void FaceOffApp::draw(){
    gl::clear( Color( 0, 0, 0 ) );
    
    //For drawing the image and keeping aspect ratio
    //https://gist.github.com/jkosoy/3895744
    
    //Draw the final image
    Rectf leftRect(0, 0, getWindowSize().x/2, getWindowSize().y) ;
    Rectf rightRect(getWindowSize().x/2, 0, getWindowSize().x, getWindowSize().y);
    
    cinder::gl::Texture2dRef leftRef = GetTextureFromMat(finalImageLeft);
    cinder::gl::Texture2dRef rightRef = GetTextureFromMat(finalImageRight);
    
    gl::draw(leftRef, Rectf(leftRef->getBounds()).getCenteredFit(leftRect, true));
    gl::draw(rightRef, Rectf(rightRef->getBounds()).getCenteredFit(rightRect, true));
    
    //gl::draw(GetTextureFromMat(finalImageLeft), Rectf(0, 0, getWindowSize().x/2, getWindowSize().y) );
    //gl::draw(GetTextureFromMat(finalImageRight), Rectf(getWindowSize().x/2, 0, getWindowSize().x, getWindowSize().y));
    
    
    //Draw the FPS counter
    char fpsString[50];
    sprintf(fpsString, "FPS: %.2f", getAverageFps());
    Font mFont = Font( "Courier", 20 );
    gl::TextureRef mTextTexture;
    vec2 mSize = vec2( 100, 100 );
    TextBox tbox = TextBox().alignment( TextBox::CENTER ).font( mFont ).size( ivec2( mSize.x, TextBox::GROW ) );
    tbox.text(fpsString);
    tbox.setColor( Color( 1.0f, 0.65f, 0.35f ) );
    tbox.setBackgroundColor( ColorA( 0, 0, 0, 0 ) );
    mTextTexture = gl::Texture2d::create( tbox.render() );
    if( mTextTexture ){
        gl::draw( mTextTexture );
    }
    
    //Draw the GUI
    GUIHandler::GetInstance().DrawAll();
    
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
