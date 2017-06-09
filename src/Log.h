#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <string>
#include <syslog.h>

typedef enum LogLevels { LOG_QUIET = 1, LOG_ERROR, LOG_MSG, LOG_DBG } LogLevel_t;
//extern LogLevel_t level;

class Log
{


        public:
                Log(int DefaultLevel);
                ~Log();
                void SetLoggingLevel(int l);
                void LogInfo(std::string msg);
                void LogDebug(std::string msg);
                void LogError(std::string msg);

        private:
                LogLevel_t level;
};
#endif
