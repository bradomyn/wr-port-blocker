#ifndef CONF_H
#define CONF_H
#include <ios>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include "Log.h"

#define NO_SIM  0
#define SIM     1


class Conf : public Log
{

    public:
        Conf(std::string config_file, int LogLevel, int Sim);
        int GetConf();
        int GetSim();
        std::map<std::string, std::string> ConfigMap = {{"firm-check-rate",""},
                                                        {"timeout-port",""},
                                                        {"wrpcsw", ""},
                                                        {"dhcp-log-date",""},
                                                        {"dhcp-log", ""}};
    private:
        std::map<std::string, std::string>::iterator cofig_it;
        std::string ConfP = "/etc/wr-port-blocker.conf";
        std::ifstream ConfF;
        std::string DhcpLog;
        int CloseConf();
        int Simulate;
};
#endif
