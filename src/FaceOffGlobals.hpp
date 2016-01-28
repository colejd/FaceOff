//
//  FaceOffGlobals.hpp
//  FaceOff
//
//  Created by Jonathan Cole on 10/27/15.
//
//

#ifndef FaceOffGlobals_hpp
#define FaceOffGlobals_hpp

#import "AppLog.hpp"

namespace FaceOffGlobals{
    /**
     When this is set to false the threads managed by this program will stop and join the main thread.
     */
    extern bool ThreadsShouldStop;
    extern FaceOffLog app_log;
}

namespace fg = FaceOffGlobals;
#endif /* FaceOffGlobals_hpp */
