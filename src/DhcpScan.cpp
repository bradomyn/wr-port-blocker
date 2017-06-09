#include "DhcpScan.h"

DhcpScan::DhcpScan(std::map<std::string, std::string> &ConfigMap, int LogLevel) : Log(LogLevel)
{
        Version = ConfigMap["wrpcsw"];
        DhcpP = ConfigMap["dhcp-log"];
        DhcpExt = ConfigMap["dhcp-log-date"];
}

int DhcpScan::OpenDhcpLog()
{
        char Datum[32];
        std::time_t Ts = std::time(nullptr);

        if (DhcpExt == "y") {
                if (std::strftime(Datum, sizeof(Datum), "%F", std::localtime(&Ts)))
                        DhcpP = DhcpP + "-" + std::string(Datum);
                else
                        return -1;
        } else {
                DhcpP = DhcpP + ".conf";
        }

        DhcpF.open(DhcpP);

        if (DhcpF) {
                LogInfo("Found and open" + DhcpP + " DHCP log file");
        }
        else {
                LogError("DHCP log file " + DhcpP + " not found");
                return (-1);
        }

        return 0;
}

int DhcpScan::CloseDhcpLog()
{
        DhcpF.close();
        LogInfo("Close " + DhcpP + " DHCP log file");
        return 0;
}

void DhcpScan::ShowBlackList(std::map<std::string, std::time_t> &BlackList)
{
        for(std::map<std::string, std::time_t>::iterator it = BlackList.begin(); it != BlackList.end(); ++it)
                LogInfo("Node with wrong Firmware version: " + std::string(it->first));
}

int DhcpScan::ScanDhcpLog(std::map<std::string, std::time_t>  &BlackList)
{
        std::string str="";
        std::string VersionNode;
        std::string Mac;
        std::time_t Ts = std::time(NULL);
        std::size_t VendorIdx;

        if (DhcpF) {
                while (std::getline(DhcpF, str))
                {
                        Ts = ConvertTS(str.substr(0,15));
                        if (CompareDate(LastLogTime, Ts)) {
                                VendorIdx = str.find("vendor class:");
                                if(VendorIdx != std::string::npos) {
                                        VersionNode = str.substr(VendorIdx + VENDOR_INIT , VendorIdx + VENDOR_END);
                                        if (VersionNode.compare(Version) != 0) {
                                                std::getline(DhcpF, str); // laziness, next line is with only info
                                                std::getline(DhcpF, str); // in this line we have IP and MAC
                                                Mac = str.substr(MAC_INIT , MAC_END);
                                                LogInfo("Node " + Mac + "with wrong firmware: " + VersionNode);
                                                BlackList[Mac] = Ts;
                                        }
                                }
                        }
                }

                LastLogTime = Ts;
        }
        else {
                LogError("Config File " + DhcpP);
                return (-1);
        }

        return 0;
}

std::time_t DhcpScan::ConvertTS(std::string Ts)
{
        struct tm tm;
        strptime(Ts.c_str(), "%B %d %H:%M:%S", &tm);

        // we don't have the year in the TS of the log file
        time_t Year = time(NULL);
        struct tm *MYear = localtime(&Year);
        tm.tm_year = MYear->tm_year;

        return mktime(&tm);
}

int DhcpScan::CompareDate(time_t SavedTS, time_t LogTime)
{
        if(SavedTS == 0) {
                return 1;
        } else {
                if(std::difftime(SavedTS, LogTime) >= 0)
                        return 0;
                else
                        return 1;
        }
}
