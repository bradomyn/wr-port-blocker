#ifndef Log
#define Log

typedef enum LogLevel { LOG_QUIET = 1, LOG_ERROR, LOG_INFO, LOG_DEBUG } LogLevel_t;

extern LogLevel_t level;

void SetLoggingLevel(int l);
void LogInfo(std::string msg);
void LogDebug(std::string msg);
void LogError(std::string msg);
#endif
