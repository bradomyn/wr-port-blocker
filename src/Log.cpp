#include "Log.h"


Log::Log(int DefaultLevel)
{
        level = (LogLevel_t)DefaultLevel;

        setlogmask (LOG_UPTO (LOG_DEBUG|LOG_INFO|LOG_ERROR));
        openlog ("wr-port-blocker.log", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_DAEMON);
}

Log::~Log()
{
        closelog ();
}

void Log::SetLoggingLevel(int l)
{
        level = (LogLevel_t)l;
}

void Log::LogInfo(std::string msg)
{
        if(level >= LOG_MSG)
                syslog(LOG_INFO, msg.c_str());
}

void Log::LogDebug(std::string msg)
{
        if(level >= LOG_DBG)
                syslog(LOG_DBG, msg.c_str());
}

void Log::LogError(std::string msg)
{
        if(level >= LOG_ERROR)
                syslog(LOG_ERROR, msg.c_str());
}

