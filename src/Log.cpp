#include <iostream>
#include "Log.h"

void SetLoggingLevel(int l) { 
    level = (LogLevel_t)l; 
}

void LogInfo(std::string msg) {
    if(level >= LOG_INFO)
        std::cout << "INFO: " << msg << std::endl;
}

void LogDebug(std::string msg) {
    if(level >= LOG_DEBUG)
        std::cout << "DEBUG: "<< msg << std::endl;
}

void LogError(std::string msg) {
    if(level >= LOG_ERROR)
    std::cerr << "ERROR: "<< msg << std::endl;
}

