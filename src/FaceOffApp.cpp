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
    if(capture1->IsInitialized()){
        capture1->StartUpdateThread();
    }
    else{
        printf("There was a problem initializing the camera capture.\n");
        exit(EXIT_FAILURE);
    }
    
    
    capture2 = new CameraCapture();
    
    /*
    capture2->Init(1, CameraCapture::DEVICE_TYPE::GENERIC);
    //Quit if the capture doesn't take
    if(capture2->IsInitialized()){
        capture2->StartUpdateThread();
    }
    else{
        printf("There was a problem initializing the camera capture.\n");
        exit(EXIT_FAILURE);
    }
     */
    
    
     
    
    
    //frame.create(cv::Size(capture.GetWidth(), capture.GetHeight()), CV_8UC3);
    
    setWindowSize(capture1->GetWidth() * 2, capture1->GetHeight());
    
    //edges = cv::Mat(cv::Size(capture.GetWidth(), capture.GetHeight()), CV_8U);
    
    SetupGUIVariables();
    
    static bool FULLSCREEN_ON_LAUNCH = ConfigHandler::GetConfig().lookup("FULLSCREEN_ON_LAUNCH");
    setFullScreen(FULLSCREEN_ON_LAUNCH);
    printf("FaceOff Init finished.\n");
    
}

void FaceOffApp::SetupGUIVariables(){
    GetGUI().GetParams()->addParam("Edge Detection", &drawEdges).keyIncr("e");
    GetGUI().GetParams()->addParam("Use Contours", &useContours).keyIncr("c");
    GetGUI().GetParams()->addParam("Low Threshold", &cannyThresholdLow).max(255).min(0);
    GetGUI().GetParams()->addParam("High Threshold", &cannyThresholdHigh).max(255).min(0);
    GetGUI().GetParams()->addParam("Contour Subdivisions", &contourSubdivisions).min(1).max(16);
    GetGUI().GetParams()->addParam("Draw cool edges", &drawCoolEdges);
}


void FaceOffApp::update(){
    
    //Grab a new frame from the camera
    //capture.Update();
    
    if(capture1->IsInitialized()){
        if(capture1->FrameIsReady()){
            //frame = capture.GetLatestFrame();
            
            cv::Mat rawFrame(capture1->GetWidth(), capture1->GetHeight(), CV_8UC3, capture1->GetLatestFrame()->data);
            cv::Mat edges;
            cv::Mat contours;
            //printf("Data1: %i\n", rawFrame.data[100]);
            //printf("Width: %i, Height: %i\n", rawFrame.cols, rawFrame.rows);
            
            //imshow("Raw Frame", rawFrame);
            
            //Do computer vision stuff
            
            if(!rawFrame.empty()){
                
                if(drawEdges){
                    if(!drawCoolEdges){
                        //Downsample
                        //cv::resize(frame, frame, cv::Size(), 0.5, 0.5, INTER_NEAREST);
                        
                        //Convert to grayscale
                        cvtColor(rawFrame, edges, COLOR_BGR2GRAY);
                        //Blur the result to reduce noise
                        GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5);
                        //Run Canny detection on the blurred image
                        Canny(edges, edges, cannyThresholdLow, cannyThresholdHigh, 3); //0, 30, 3
                        
                        //cv::resize(edges, edges, cv::Size(), 2.0, 2.0, INTER_NEAREST);
                        
                        if(useContours) cv::parallel_for_(cv::Range(0, contourSubdivisions), ParallelContourDetector(edges, edges, contourSubdivisions));
                        
                        cvtColor(edges, finalImageLeft, cv::COLOR_GRAY2BGR);
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
                        cvtColor(result, finalImageLeft, cv::COLOR_GRAY2BGR);
                    }
                 
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
                    
                    if(useContours) cv::parallel_for_(cv::Range(0, contourSubdivisions), ParallelContourDetector(edges, edges, contourSubdivisions));
                    
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
    Rectf leftRect(0, 0, getWindowSize().x/2, getWindowSize().y) ;
    Rectf rightRect(getWindowSize().x/2, 0, getWindowSize().x, getWindowSize().y);
    cinder::gl::Texture2dRef leftRef = GetTextureFromMat(finalImageLeft);
    cinder::gl::Texture2dRef rightRef = GetTextureFromMat(finalImageRight);
    
    //Rectf(leftRef.getBounds()).getCenteredFit(leftRect, true)
    gl::draw(leftRef, Rectf(leftRef->getBounds()).getCenteredFit(leftRect, true));
    gl::draw(rightRef, Rectf(rightRef->getBounds()).getCenteredFit(rightRect, true));
        //gl::draw(GetTextureFromMat(finalImageLeft), Rectf(0, 0, getWindowSize().x/2, getWindowSize().y) );
        //gl::draw(GetTextureFromMat(finalImageRight), Rectf(getWindowSize().x/2, 0, getWindowSize().x, getWindowSize().y));
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

void ParallelContourDetector::operator ()(const cv::Range &range) const{
    using namespace cv;
    
    for(int i = range.start; i < range.end; i++){
        vector< vector<cv::Point> > contourData;
        vector<Vec4i> contourHierarchy;
        cv::Mat in(src_gray, cv::Rect(0, (src_gray.rows/subsections)*i, src_gray.cols, src_gray.rows/subsections));
        cv::Mat out(out_gray, cv::Rect(0, (out_gray.rows/subsections)*i, out_gray.cols, out_gray.rows/subsections) );
        try{
            //C++ API
            
            findContours( in, contourData, contourHierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
            out = Scalar::all(0);
            Scalar color = Scalar(255, 255, 255, 255);
            //srand (time(NULL));
            int thickness = 3;
            //drawContours(out, contourData, -1, color, thickness, 8, contourHierarchy);
            for (vector<cv::Point> contour : contourData) {
                if(true) color = Scalar(rand()&255, rand()&255, rand()&255);
                polylines(out, contour, true, color, thickness, 8);
            }
            
            //C API
            /*
            CvMemStorage *mem;
            mem = cvCreateMemStorage(0);
            CvSeq *contours = 0;
            //Find contours in canny_output
            //IplImage convertedCanny = (in.getMat(ACCESS_READ));
            //IplImage* convertedCanny = new IplImage(in.getMat(0));
            
            IplImage* convertedCanny;
            convertedCanny = cvCreateImage(cvSize(in.cols,in.rows),8,3);
            IplImage ipltemp=in;
            cvCopy(&ipltemp,convertedCanny);
            
            cvFindContours(&convertedCanny, mem, &contours, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
            //Draw contours in contour_output
            IplImage* convertedContours = cvCreateImage(cvSize(out.size().width, out.size().height), 8, 3);
            cvZero(convertedContours);
            
            cv::Scalar lineColor = cv::Scalar(255, 0, 0);
            cv::Scalar holeColor = cv::Scalar(0, 0, 0);
            cvDrawContours(convertedContours, contours, lineColor,
                           holeColor,
                           100, thickness);
            //printf("%s\n", type2str(finalContours.type()).c_str());
            //printf("%i\n", finalContours.rows * finalContours.cols );
            
            //Convert image back to UMat
            
            Mat finalContours;
            finalContours.create(out.rows, out.cols, CV_8UC3);
            finalContours = cvarrToMat(convertedContours);
            finalContours.copyTo(out);
            finalContours.release();
            
            //cvRelease(contours);
            
            cvClearMemStorage(mem);
            cvReleaseImage(&convertedContours);
            cvReleaseImage(&convertedCanny);
            //cvReleaseImage(&(&convertedCanny));
             */
            
        }
        catch(...){
            printf("[CVEye] ParallelContourDetector error.\n");
        }
        
        in.release();
        out.release();
    }
    
}

ParallelContourDetector::~ParallelContourDetector(){ }

CINDER_APP( FaceOffApp, RendererGl, prepareSettings )
