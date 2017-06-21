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



class Lldp : public Log
{
        public:
                Lldp(int DebugLevel);
                ~Lldp();
                int GetLldp(void);
                int GetNodesInSwitch();
                void ShowSwitch(void);
                void snmp_get_and_print(netsnmp_session * ss, oid * theoid, size_t theoid_len, int pdu_t);
                //lldpctl_send_callback send(lldpctl_conn_t *conn, const uint8_t *data, size_t length, void *user_data);
        private:
                std::map<std::string, std::time_t> SwitchList;
                int numprinted;
};
#endif
