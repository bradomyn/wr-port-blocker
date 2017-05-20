#ifndef Log
#define Log

enum LogLevel { LOG_QUIET, LOG_ERROR, LOG_INFO, LOG_DEBUG };

extern LogLevel level;

void SetLoggingLevel(LogLevel);
void LogInfo(std::string msg);
void LogDebug(std::string msg);
void LogError(std::string msg);
#endif
