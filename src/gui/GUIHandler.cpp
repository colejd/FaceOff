//
//  GUIHandler.cpp
//  FaceOff
//
//  Created by Jonathan Cole on 10/21/15.
//
//

#include "GUIHandler.hpp"

GUIHandler::GUIHandler(){
    mParams = params::InterfaceGl::create( getWindow(), "General Settings", toPixels( ivec2( 200, 100 ) ) );
    mParams->setPosition(ivec2(0, 50));
}

void GUIHandler::Draw(){
    mParams->draw();
}

params::InterfaceGlRef GUIHandler::GetParams(){
    return mParams;
}