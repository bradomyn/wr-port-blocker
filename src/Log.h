#ifndef Log
#define Log

#include <iostream>
#include <string>

typedef enum LogLevels { LOG_QUIET = 1, LOG_ERROR, LOG_MSG, LOG_DBG } LogLevel_t;

extern LogLevel_t level;

void SetLoggingLevel(int l);
void LogInfo(std::string msg);
void LogDebug(std::string msg);
void LogError(std::string msg);
#endif
