#ifndef Conf_H
#define Conf_H
#include <ios>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include "Log.h"

class Conf {

    public:
        Conf(std::string config_file);
        int GetConf();

    private:
        std::map<std::string, std::string>::iterator cofig_it;
        std::string Conf_p = "/etc/wr-port-blocker.conf";
        std::string wr_version;
        std::ifstream Conf_f;
        std::string dhcp_log;
        int firm_check_rate;
        int time_out_port;
        int CloseConf();        
        std::map<std::string, std::string> config_map = {{"firm-check-rate",""}, 
                                                        {"timeout-port",""}, 
                                                        {"logging",""}, 
                                                        {"ip-syslog",""}, 
                                                        {"wrpcsw", ""}, 
                                                        {"dhcp-log", ""}};
};
#endif
