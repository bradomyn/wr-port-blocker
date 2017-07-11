#ifndef BLOCKER_H
#define BLOCKER_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
//#include "Conf.h"
#include "Log.h"
#include "Daemonize.h"
#include "SshConn.h"
#include "DhcpScan.h"
#include "Lldp.h"

class Blocker : public Log
{
        public:
                Blocker(std::map<std::string, std::string> &ConfigMap, int LogLevel, boost::asio::io_service& io);
                ~Blocker();
                int Run();
        //private:
                int BlockerLog;
                std::map<std::string, std::time_t> BlackList;
                DhcpScan *Scan;
                SshConn *Ssh;
                Lldp *LldpCrawl;
                boost::asio::deadline_timer timer_;
};
#endif
