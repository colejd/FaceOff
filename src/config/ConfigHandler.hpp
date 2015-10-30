//
//  ConfigHandler.hpp
//  FaceOff
//
//  Created by Jonathan Cole on 10/21/15.
//
//

#ifndef ConfigHandler_hpp
#define ConfigHandler_hpp

#include <libconfig.h++>

using namespace std;
using namespace libconfig;

/**
 Manages the config file for the program.
 THIS IS NOT THREAD SAFE IN C++03 -- USE C++11!
 */
class ConfigHandler {
protected:
    //==== SINGLETON STUFF ==============================================//
    static ConfigHandler& GetInstance()
    {
        static ConfigHandler instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    //==== END SINGLETON STUFF ==============================================//
    
public:
    
    /**
     Get the config statically using fewer commands
     */
    static Config& GetConfig(){
        return GetInstance().config;
    }
    
    Config config;
    
private:
    //==== SINGLETON STUFF ==============================================//
    ConfigHandler();
    // C++11:
    // Stop the compiler from generating copy methods for the object
    ConfigHandler(ConfigHandler const&) = delete;
    void operator=(ConfigHandler const&) = delete;
    //==== END SINGLETON STUFF ==============================================//
    
    const char *CONFIG_FILE = "FaceOff.cfg";
};






#endif /* ConfigHandler_hpp */
