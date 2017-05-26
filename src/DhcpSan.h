#ifndef DHCPSAN_H
#define DHCPSAN_H
#include <ios>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <iostream>
#include <ctime>
#include <time.h>
#include "Log.h"

#define VENDOR_INIT     14
#define VENDOR_END      32
#define MAC_INIT        73
#define MAC_END         90

class DhcpScan
{
        public:
                DhcpScan(std::map<std::string, std::string> &ConfigMap);
                int ScanDhcpLog(std::map<std::string, std::time_t> &BlackList);
                int CloseDhcpLog();
                int OpenDhcpLog();
                void ShowBlackList(std::map<std::string, std::time_t> &BlackList);
        private:
                int CompareDate(time_t SavedTS, time_t LogTime);
                time_t ConvertTS(std::string Ts);
                std::string DhcpP;
                std::ifstream DhcpF;
                std::time_t LastLogTime = 0;
                std::string Version;
                std::string DhcpExt;
};
#endif
