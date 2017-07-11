#include "Blocker.h"

Blocker::Blocker(std::map<std::string, std::string> &ConfigMap,
                 int LogLevel, boost::asio::io_service& io) : Log(LogLevel),
                 timer_(io, boost::posix_time::seconds(5))
{
        BlockerLog = LogLevel;
        Ssh = new SshConn(LogLevel);
        Ssh->Config();
        LldpCrawl = new Lldp(LogLevel);
        Scan = new DhcpScan(ConfigMap, LogLevel);
        timer_.async_wait(boost::bind(&Blocker::Run, this));
}

Blocker::~Blocker()
{

}

int Blocker::Run()
{

        //Lldp.ShowSwitch();

        //SshConn.SendCommand("ls");

        LldpCrawl->GetLldp();
        Scan->OpenDhcpLog();

        Scan->ScanDhcpLog(BlackList);


        //scan.ShowBlackList(BlackList);
        Scan->CloseDhcpLog();

        timer_.expires_at(timer_.expires_at() + boost::posix_time::seconds(5));
        timer_.async_wait(boost::bind(&Blocker::Run, this));
        return 0;
}
