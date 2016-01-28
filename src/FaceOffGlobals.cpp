//
//  FaceOffGlobals.cpp
//  FaceOff
//
//  Created by Jonathan Cole on 10/27/15.
//
//

#include "FaceOffGlobals.hpp"

#import "AppLog.hpp"

namespace FaceOffGlobals{
    /**
     When this is set to false the threads managed by this program will stop and join the main thread.
     */
    bool ThreadsShouldStop = false;
    
    /**
     Logger which will show output on the Log window in the application.
     */
    FaceOffLog app_log;
}