#include <iostream>
#include <string>
#include <vector>
#include "Conf.h"
#include "Log.h"
#include "Daemonize.h"

LogLevel_t level = LOG_ERROR;

static void show_usage(std::string name)
{
    std::cerr << "Usage: " << name << " <option(s)> "
              << "Options:\n"
              << "\t-h,--help\tShow this help message\n"
              << "\t-v,\t\tinfo verbosity level\n"
              << "\t-vv,\t\tdebug verbosity level\n"
              << "\t-c,--config  CONFIGFILE Specify path to config file\n"
              << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc < 1) {
        show_usage(argv[0]);
        return 1;
    }

    std::vector <std::string> sources;
    std::string config_file;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        } else if ((arg == "-c") || (arg == "--config")) {
            if (i + 1 < argc) {
                config_file = argv[i+1];
            } else {
                LogError("--config option requires one argument.");
                return 1;
            }  
        } else if (arg == "-vv") {
            SetLoggingLevel(LOG_DEBUG);
            LogDebug("Verbosity Level: Debug");
        } else if (arg == "-v") {
            SetLoggingLevel(LOG_INFO);
            LogInfo("Verbosity Level: Info");
        }else {
            sources.push_back(argv[i]);
        }
    }
    
    
    // init 
    Conf Conf(config_file);
    if (Conf.GetConf() < 0) {
        LogError("Without Config information, I don't start, Ciao");
        return -1;
    } else
        LogInfo("Config File loaded correctly");
    
    //Daemonize();
    //block = BlockerEngine(Conf);
    //
 
    return 0;
}
