#include "FaceOffApp.h"

#include "CinderImGui.h"

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
    
    //Load config stuff
    static int WINDOW_WIDTH = ConfigHandler::GetConfig().lookup("DEFAULT_WINDOW_WIDTH");
    static int WINDOW_HEIGHT = ConfigHandler::GetConfig().lookup("DEFAULT_WINDOW_HEIGHT");
    static bool USE_OPENCL = ConfigHandler::GetConfig().lookup("USE_OPENCL");
    static bool USE_DEDICATED_GPU = ConfigHandler::GetConfig().lookup("USE_DEDICATED_GPU");
    
    //Set up app window
    setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    //Enable OpenCL acceleration based on config file value
    cv::ocl::setUseOpenCL(USE_OPENCL);
    
    //http://answers.opencv.org/question/62635/opencv-30-how-to-determine-which-gpu-is-used-for-opencl/
    //Create an OpenCL context on the dedicated GPU and use it for this program if requested by config
    if(USE_OPENCL && USE_DEDICATED_GPU){
        cv::ocl::Context context;
        if (!context.create(cv::ocl::Device::TYPE_DGPU)) {
            cout << "Failed creating the dedicated GPU context!" << endl;
            //return;
        }
            
        cout << context.ndevices() << " GPU devices are detected." << endl;
        //Enumerate all GPU devices in the computer
        for (int i = 0; i < context.ndevices(); i++) {
            cv::ocl::Device device = context.device(i);
            std::cout.setf(std::ios::boolalpha);
            cout << "name: " << device.name() << endl;
            cout << "available: " << device.available() << endl;
            cout << "imageSupport: " << device.imageSupport() << endl;
            cout << "OpenCL_C_Version: " << device.OpenCL_C_Version() << endl;
            cout << endl;
        }
        
        //Set the GPU to use
        cv::ocl::Device(context.device(0));
    }
    
    //Print debug info
    PrintDebugInfo();
    
    
    
    //Set up camera device
    capture1 = new CameraCapture();
    capture1->Init(0, CameraCapture::DEVICE_TYPE::GENERIC);
    //Quit if the capture doesn't take
    if(!capture1->IsInitialized()){
        printf("Camera is not initialized.\n");
        exit(EXIT_FAILURE);
    }
    
    
    capture2 = new CameraCapture();
    capture2->Init(1, CameraCapture::DEVICE_TYPE::PS3EYE);
    //Quit if the capture doesn't take
    if(!capture2->IsInitialized()){
        printf("Camera is not initialized.\n");
        //exit(EXIT_FAILURE);
    }
    
    edgeDetector = EdgeDetectorModule();
    faceDetector = FaceDetectorModule();
    
    setWindowSize(capture1->GetWidth() * 2, capture1->GetHeight());
    
    SetupGUIVariables();
    
    //Sets the window to fullscreen if requested by config. Do this at the end of the initialization
    //to avoid nullifying any calls to setWindowSize called after fullscreen is requested.
    static bool FULLSCREEN_ON_LAUNCH = ConfigHandler::GetConfig().lookup("FULLSCREEN_ON_LAUNCH");
    setFullScreen(FULLSCREEN_ON_LAUNCH);
    printf("FaceOff Init finished.\n");
    
}

/**
 Print information relevant to Cinder, OpenCV, etc.
 */
void FaceOffApp::PrintDebugInfo(){
    std::cout << "--- BEGIN DEBUG INFO ---\n";
    
    //Set stdout to print bools as true/false rather than 1/0
    std::cout.setf(std::ios::boolalpha);
    
    #ifdef DEBUG
        std::cout << "Debug build\n";
        //std::cout << cv::getBuildInformation();
    #else
        std::cout << "Release build\n";
    #endif
    
    bool openCLSupport = ocl::useOpenCL();
    std::cout << "OpenCL support: " << openCLSupport << "\n";
    //#ifdef DEBUG
        if(openCLSupport){
            std::vector<cv::ocl::PlatformInfo> platforms;
            cv::ocl::getPlatfomsInfo(platforms);
            for(int i=0; i<platforms.size(); i++){
                printf("  Platform %i of %lu\n", i+1, platforms.size());
                printf("    Name:     %s\n", platforms[i].name().c_str());
                printf("    Vendor:   %s\n", platforms[i].vendor().c_str());
                printf("    Device:   %i\n", platforms[i].deviceNumber());
                printf("    Version:  %s\n", platforms[i].version().c_str());
            }
        }
    //#endif
    
    std::cout << "Optimized code support: " << useOptimized() << "\n";
    
    std::cout << "IPP support: " << cv::ipp::useIPP() << "\n";
    
    std::cout << "Threads used by OpenCV: " << getNumThreads() << "\n";
    
    std::cout << "--- END DEBUG INFO ---\n";
}

void FaceOffApp::SetupGUIVariables(){
    
    ui::initialize();
    
    //Set up global preferences for the GUI
    //http://anttweakbar.sourceforge.net/doc/tools:anttweakbar:twbarparamsyntax
    GetGUI().DefineGlobal("help = ' FaceOff 0.1.0 \n Jonathan Cole | Toni Kaplan'");
    GetGUI().DefineGlobal("iconalign=horizontal");
    
    params::InterfaceGlRef generalWindow = GetGUI().GetWindow("General Settings");
    generalWindow->addParam("Stereo Convergence", &convergence).min(0).max(100).keyDecr("-").keyIncr("=");
    
    
}


void FaceOffApp::update(){
    
    //Grab a new frame from the camera
    
    if(capture1->IsInitialized()){
        if(capture1->FrameIsReady()){
            //Explicitly make a new Mat with the properties of the capture and the data of the capture's latest frame.
            //We do this because capture->GetLatestFrame returns a Mat with an invalid header but correct data.
            capture1->LockFrame();
            cv::Mat rawFrame(capture1->GetWidth(), capture1->GetHeight(), CV_8UC3, capture1->GetLatestFrame().data);
            capture1->UnlockFrame();
            
            //cv::UMat rawFrameGPU;
            //rawFrame.copyTo(rawFrameGPU);
            
            edgeDetector.ProcessFrame(rawFrame, rawFrame);
            faceDetector.ProcessFrame(rawFrame, rawFrame);
            //finalImageLeft = edgeDetector.ProcessFrame(capture1->GetLatestFrame());
            
            rawFrame.copyTo(finalImageLeft);
            //More or less a mutex unlock for capture->GetLatestFrame()
            capture1->MarkFrameUsed();
            //rawFrame.release();
            //rawFrameGPU.release();
        }
    }
    
    if(capture2->IsInitialized()){
        if(capture2->FrameIsReady()){
            //Explicitly make a new Mat with the properties of the capture and the data of the capture's latest frame.
            //We do this because capture->GetLatestFrame returns a Mat with an invalid header but correct data.
            cv::Mat rawFrame(capture2->GetWidth(), capture2->GetHeight(), CV_8UC3, capture2->GetLatestFrame().data);
            
            edgeDetector.ProcessFrame(rawFrame, rawFrame);
            faceDetector.ProcessFrame(rawFrame, rawFrame);
            
            rawFrame.copyTo(finalImageRight);
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
    Rectf leftRect(0 + convergence, 0, (getWindowSize().x/2) + convergence, getWindowSize().y) ;
    Rectf rightRect((getWindowSize().x/2) - convergence, 0, getWindowSize().x - convergence, getWindowSize().y);
    
    cinder::gl::Texture2dRef leftRef = GetTextureFromMat(finalImageLeft);
    cinder::gl::Texture2dRef rightRef = GetTextureFromMat(finalImageRight);
    
    if(swapEyes){
        gl::draw(rightRef, Rectf(leftRef->getBounds()).getCenteredFit(leftRect, true));
        gl::draw(leftRef, Rectf(rightRef->getBounds()).getCenteredFit(rightRect, true));
    }
    else{
        gl::draw(leftRef, Rectf(leftRef->getBounds()).getCenteredFit(leftRect, true));
        gl::draw(rightRef, Rectf(rightRef->getBounds()).getCenteredFit(rightRect, true));
    }
    
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
    if( event.getChar() == 's' ) {
        swapEyes = !swapEyes;
    }
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
