#ifndef LLDP_H
#define LLDP_H

#include <iostream>
#include <stdint.h>
#include <vector>
#include <map>
#include <ctime>
#include <lldpctl.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include "Log.h"

#define NETSNMP_DS_WALK_INCLUDE_REQUESTED	        1
#define NETSNMP_DS_WALK_PRINT_STATISTICS	        2
#define NETSNMP_DS_WALK_DONT_CHECK_LEXICOGRAPHIC	3
#define NETSNMP_DS_WALK_TIME_RESULTS     	        4
#define NETSNMP_DS_WALK_DONT_GET_REQUESTED	        5
#define NETSNMP_DS_WALK_TIME_RESULTS_SINGLE	        6

class Lldp : public Log
{
        public:
                Lldp(int DebugLevel);
                ~Lldp();
                int GetLldp(void);
                int GetNodesInSwitch();
                void ShowSwitch(void);
                void GetFirstOID(netsnmp_session * ss, oid * theoid, size_t theoid_len, int pdu_t);
                //lldpctl_send_callback send(lldpctl_conn_t *conn, const uint8_t *data, size_t length, void *user_data);
        private:
                std::map<std::string, std::time_t> SwitchList;
                const std::string LldpOIDMac = "iso.0.8802.1.1.2.1.4.1.1.5";
                int numprinted;
                netsnmp_session *ss;
                netsnmp_session session;
                int SnmpDebug;
};
#endif
