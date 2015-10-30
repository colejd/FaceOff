//
//  GUIHandler.hpp
//  FaceOff
//
//  Created by Jonathan Cole on 10/21/15.
//
//

//https://github.com/cinder/Cinder/blob/master/samples/ParamsBasic/src/ParamsBasicApp.cpp
//https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
//https://stackoverflow.com/questions/1661529/is-meyers-implementation-of-singleton-pattern-thread-safe


#ifndef GUIHandler_hpp
#define GUIHandler_hpp

#include <string>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;

/**
 Singleton that manages the GUI for the program.
 THIS IS NOT THREAD SAFE IN C++03 -- USE C++11!
 */
class GUIHandler {
    
public:
    //==== SINGLETON STUFF ==============================================//
    static GUIHandler& GetInstance()
    {
        static GUIHandler instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    //==== END SINGLETON STUFF ==============================================//
    
    void DrawWindow(string name);
    void DrawAll();
    
    void AddWindow(string name, ivec2 size);
    params::InterfaceGlRef GetWindow(string name);
    
    
    
private:
    //==== SINGLETON STUFF ==============================================//
    GUIHandler();
    // C++11:
    // Stop the compiler from generating copy methods for the object
    GUIHandler(GUIHandler const&) = delete;
    void operator=(GUIHandler const&) = delete;
    //==== END SINGLETON STUFF ==============================================//
    
    params::InterfaceGlRef mParams;
    
    unordered_map<string, params::InterfaceGlRef> windows;
    
};

#endif /* GUIHandler_hpp */
