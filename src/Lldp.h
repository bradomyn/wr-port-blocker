#ifndef LLDP_H
#define LLDP_H

#include <iostream>
#include <stdint.h>
#include <vector>
#include <map>
#include <ctime>
#include <lldpctl.h>
#include <sstream>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <stdexcept>
#include "Log.h"

#define NETSNMP_DS_WALK_INCLUDE_REQUESTED	        1
#define NETSNMP_DS_WALK_PRINT_STATISTICS	        2
#define NETSNMP_DS_WALK_DONT_CHECK_LEXICOGRAPHIC	3
#define NETSNMP_DS_WALK_TIME_RESULTS     	        4
#define NETSNMP_DS_WALK_DONT_GET_REQUESTED	        5
#define NETSNMP_DS_WALK_TIME_RESULTS_SINGLE	        6


typedef std::map<std::string, unsigned> NodePortMap;

class Lldp : public Log
{
        public:
                Lldp(int DebugLevel);
                ~Lldp();
                int GetLldp(void);
                NodePortMap GetNodesInSwitch(std::string &WRSwitch);
                void ShowSwitch(void);
                void GetFirstOID(netsnmp_session * ss, oid * theoid, size_t theoid_len, int pdu_t);

        private:
                std::map<std::string, NodePortMap> SwitchNode;
                NodePortMap NodePort;
                const std::string LldpOIDMac = "iso.0.8802.1.1.2.1.4.1.1.5";
                int numprinted;
                netsnmp_session *ss;
                netsnmp_session session;
                int SnmpDebug;
};
#endif
