#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Conf.h"
#include "Log.h"
#include "Daemonize.h"
#include "SshConn.h"
#include "DhcpScan.h"
#include "Lldp.h"
#include "Blocker.h"

static void show_usage(std::string name)
{
        std::cerr << "Usage: " << name << " <option(s)> "
                << "Options:\n"
                << "\t-h,--help\tShow this help message\n"
                << "\t-v,\t\tinfo verbosity level\n"
                << "\t-vv,\t\tdebug verbosity level\n"
                << "\t-s,\t\tsimulate operation of blocking\n"
                << "\t-d \t\tdaemonize \n"
                << "\t-c,--config  CONFIGFILE Specify path to config file\n"
                << std::endl;
}

int main(int argc, char* argv[])
{
        boost::asio::io_service io;
        std::vector <std::string> sources;
        std::string config_file;
        Log Log(LOG_ERROR);
        int level = LOG_ERROR;
        int Sim = NO_SIM;

        int daemonize=0;

        if (argc < 1) {
                show_usage(argv[0]);
                return 1;
        }

        for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                if ((arg == "-h") || (arg == "--help")) {
                        show_usage(argv[0]);
                        return 0;
                } else if ((arg == "-c") || (arg == "--config")) {
                        if (i + 1 < argc) {
                                config_file = argv[i+1];
                        } else {
                                Log.LogError("--config option requires one argument.");
                                return 1;
                        }
                } else if (arg == "-s"){
                        Log.LogInfo("Simulate operation");
                        Sim = SIM;
                } else if (arg == "-d") {
                        Log.LogInfo("Deamonize");
                        daemonize = 1;
                } else if (arg == "-vv") {
                        Log.SetLoggingLevel(LOG_DBG);
                        Log.LogDebug("Verbosity Level: Debug");
                        level = LOG_DBG;
                } else if (arg == "-v") {
                        Log.SetLoggingLevel(LOG_MSG);
                        Log.LogInfo("Verbosity Level: Info");
                        level = LOG_MSG;
                }else {
                        sources.push_back(argv[i]);
                }
        }


        // init
        Conf Conf(config_file, level, Sim);
        if (Conf.GetConf() < 0) {
                Log.LogError("Without Config information, I don't start, Ciao");
                return -1;
        }

        if (daemonize)
                Daemonize();

        Blocker Block(Conf.ConfigMap, level, io);
        io.run();

        //SshConn SshConn(level);
        //Lldp Lldp(level);
        //Lldp.GetLldp();
        //Lldp.ShowSwitch();
        ////SshConn.Config();
        ////SshConn.SendCommand("ls");
        //DhcpScan scan(Conf.ConfigMap, level);
        //scan.OpenDhcpLog();
        //std::map<std::string, std::time_t> BlackList;
        //scan.ScanDhcpLog(BlackList);
        ////scan.ShowBlackList(BlackList);
        //scan.CloseDhcpLog();

        return 0;
}
