//
//  GUIHandler.cpp
//  FaceOff
//
//  Created by Jonathan Cole on 10/21/15.
//
//

#include "GUIHandler.hpp"

GUIHandler::GUIHandler(){
    AddWindow("General Settings", ivec2(200, 150));
    GetWindow("General Settings")->setPosition(ivec2(5, 50));
}

/**
 Adds a window to the GUI with the given name and size.
 */
void GUIHandler::AddWindow(string name, ivec2 size){
    params::InterfaceGlRef newWindow = params::InterfaceGl::create( getWindow(), name, toPixels( size ) );
    windows.insert(std::make_pair(name, newWindow));
}

/**
 Returns a handle for the window with the specified name.
 Gives the default window if the name doesn't exist.
 */
params::InterfaceGlRef GUIHandler::GetWindow(string name){
    if(windows.count(name) > 0){
        return windows.at(name);
    }
    else{
        return windows.at("General Settings");
    }
}

/**
 Invokes a draw call on the window with the specified name.
 */
void GUIHandler::DrawWindow(string name){
    GetWindow(name)->draw();
}

/**
 Invokes a draw call on every registered window.
 */
void GUIHandler::DrawAll(){
    for (const auto& i : windows)
    {
        i.second->draw();
    }
}