#include "Lldp.h"

Lldp::Lldp(int DebugLevel) : Log(DebugLevel)
{
        init_snmp("snmpapp");
        snmp_sess_init( &session );
        SnmpDebug = DebugLevel;
}

Lldp::~Lldp()
{
        snmp_close(ss);
}

void Lldp::GetFirstOID(netsnmp_session * ss, oid * theoid, size_t theoid_len, int pdu_t)
{
    netsnmp_pdu    *pdu, *response;
    netsnmp_variable_list *vars;
    int             status;

    pdu = snmp_pdu_create(SNMP_MSG_GET);
    snmp_add_null_var(pdu, theoid, theoid_len);

    status = snmp_synch_response(ss, pdu, &response);

    if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
        for (vars = response->variables; vars; vars = vars->next_variable) {
            numprinted++;
            if(SnmpDebug >= LOG_DBG)
                print_variable(vars->name, vars->name_length, vars);
        }
    }

    if (response) {
        snmp_free_pdu(response);
    }
}


NodePortMap Lldp::GetNodesInSwitch(std::string &WRSwitch)
{
        netsnmp_pdu *pdu;
        netsnmp_pdu *response;
        NodePortMap NodePort;

        oid anOID[MAX_OID_LEN];
        oid end_OID[MAX_OID_LEN];
        oid next_OID[MAX_OID_LEN];
        size_t anOID_len = MAX_OID_LEN;
        size_t end_len;
        size_t next_len;
        int count;
        struct timeval tv_a, tv_b;

        struct variable_list *vars;
        int status;
        int check;
        int running;
        char ip[16];
        unsigned char pub[] = "public";

        strncpy(ip, WRSwitch.c_str(), sizeof(ip));
        ip[sizeof(ip) - 1] = 0;
        session.peername = ip;

        /* set the SNMP version number */
        session.version = SNMP_VERSION_2c;

        /* set the SNMPv2c community name used for authentication */
        session.community = pub;
        session.community_len = strlen((const char*)session.community);

        anOID_len = MAX_OID_LEN;

        if (!snmp_parse_oid(LldpOIDMac.c_str(), anOID, &anOID_len)) {
                throw std::runtime_error(LldpOIDMac.c_str());
        }


        memmove(next_OID, anOID, anOID_len * sizeof(oid));
        next_len = anOID_len;


        memmove(end_OID, anOID, anOID_len * sizeof(oid));
        end_len = anOID_len;
        end_OID[end_len-1]++;


        ss = snmp_open(&session);

        if (ss == NULL) {
                throw std::runtime_error("No session open, something horrible happened!!!");
        }

        check = !netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID,
                                        NETSNMP_DS_WALK_DONT_CHECK_LEXICOGRAPHIC);

        GetFirstOID(ss, anOID, anOID_len, SNMP_MSG_GET);

        running = 1;

        while(running) {

                pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
                snmp_add_null_var(pdu, next_OID, next_len);

                if (netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_WALK_TIME_RESULTS_SINGLE))
                        netsnmp_get_monotonic_clock(&tv_a);

                status = snmp_synch_response(ss, pdu, &response);

                if (status == STAT_SUCCESS) {

                        if (netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_WALK_TIME_RESULTS_SINGLE)) {
                                netsnmp_get_monotonic_clock(&tv_b);
                        }

                        if (response->errstat == SNMP_ERR_NOERROR) {
                                for (vars = response->variables; vars; vars = vars->next_variable) {

                                        if (snmp_oid_compare(end_OID, end_len, vars->name, vars->name_length) <= 0) {
                                                running = 0;
                                                continue;
                                        }

                                        numprinted++;

                                        if (netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_WALK_TIME_RESULTS_SINGLE)) {
                                                if(SnmpDebug >= LOG_DBG)
                                                        fprintf(stdout, "%f s: ",
                                                                (double) (tv_b.tv_usec - tv_a.tv_usec)/1000000 +
                                                                (double) (tv_b.tv_sec - tv_a.tv_sec));
                                        }

                                        unsigned SwitchPort = vars->name_loc[vars->name_length - 2] - 2;

                                        if( SwitchPort != 0) {

                                                std::string Mac;
                                                char MacText[4];
                                                unsigned long MacHex;

                                                for (int t=0; t <= 40; t = t + 8) {
                                                        MacHex = ((*vars->val.integer & (0xFFL << t)) >> t);
                                                        sprintf(MacText,"%02lx",MacHex);
                                                        Mac.append(MacText);
                                                        if (t + 8 <= 40)
                                                                Mac.append(":");
                                                }

                                                LogInfo("\t WR Node: " + Mac + " in port:" + std::to_string(SwitchPort));

                                                NodePort[Mac] = SwitchPort;

                                                if(SnmpDebug >= LOG_DBG)
                                                        print_variable(vars->name, vars->name_length, vars);
                                        }

                                        if ((vars->type != SNMP_ENDOFMIBVIEW) &&
                                                        (vars->type != SNMP_NOSUCHOBJECT) &&
                                                        (vars->type != SNMP_NOSUCHINSTANCE)) {
                                                if (check && snmp_oid_compare(next_OID, next_len,
                                                                        vars->name,
                                                                        vars->name_length) >= 0) {
                                                        if(SnmpDebug >= LOG_DBG) {
                                                                fprintf(stderr, "Error: OID not increasing: ");
                                                                fprint_objid(stderr, next_OID, next_len);
                                                                fprintf(stderr, " >= ");
                                                                fprint_objid(stderr, vars->name, vars->name_length);
                                                                fprintf(stderr, "\n");
                                                        }
                                                        running = 0;
                                                }
                                                memmove((char *) next_OID, (char *) vars->name,
                                                                vars->name_length * sizeof(oid));
                                                next_len = vars->name_length;
                                        } else
                                                running = 0;
                                }
                        } else {
                                running = 0;
                                if (response->errstat == SNMP_ERR_NOSUCHNAME) {
                                        LogDebug("SNMP: End of MIB");
                                } else {
                                       if (response->errindex != 0) {
                                                LogError("SNMP: Failed object: ");
                                                for (count = 1, vars = response->variables;
                                                                vars && count != response->errindex;
                                                                vars = vars->next_variable, count++)
                                                if (vars)
                                                        if(SnmpDebug >= LOG_DBG) {
                                                                fprint_objid(stderr, vars->name,
                                                                        vars->name_length);
                                                        }
                                        }
                                        throw std::runtime_error("Error in packet. Reason: " +
                                                                  std::string(snmp_errstring(response->errstat)));
                                }
                        }
                } else if (status == STAT_TIMEOUT) {
                        running = 0;
                        throw std::runtime_error("Timeout No Response");
                } else {
                        running = 0;
                }

                if (response)
                        snmp_free_pdu(response);
        }

        return NodePort;
}

int Lldp::GetLldp(void)
{
        const char *ctlname = NULL;
        lldpctl_conn_t *conn = NULL;
        lldpctl_atom_t *port;
        lldpctl_atom_t *neighbors;
        lldpctl_atom_t *neighbor;
        lldpctl_atom_t *mgmts;
        lldpctl_atom_t *mgmt;
        char *user_data = NULL;
        const char *port_num;

        ctlname = lldpctl_get_default_transport();
        conn = lldpctl_new_name(ctlname, NULL, NULL, user_data);

        if (conn == NULL) {
                LogError("LLDP: Fail to get start liblldp and connect to lldpd daemon");
                return -1;
        }

        LogInfo("LLDP: Started liblldp and connected to lldpd daemon");

        lldpctl_atom_t *iface, *ifaces;

        ifaces = lldpctl_get_interfaces(conn);

        if (ifaces == NULL)
                LogDebug("LLDP: NULL iface list");

        lldpctl_atom_foreach(ifaces, iface) {
                port = lldpctl_get_port(iface);
                neighbors = lldpctl_atom_get(port, lldpctl_k_port_neighbors);
                lldpctl_atom_foreach(neighbors, neighbor) {
                        lldpctl_atom_t *chassis = lldpctl_atom_get(neighbor, lldpctl_k_port_chassis);
                        port_num =  lldpctl_atom_get_str(chassis, lldpctl_k_chassis_index);
                        if (port_num != NULL) {
                                std::string Chassis(lldpctl_atom_get_str(chassis, lldpctl_k_chassis_name));
                                std::string Interface(lldpctl_atom_get_str(iface, lldpctl_k_interface_name));
                                std::string Index(lldpctl_atom_get_str(chassis, lldpctl_k_chassis_index));
                                std::string Mac(lldpctl_atom_get_str(chassis, lldpctl_k_chassis_id));

                                LogDebug("Device: " + Chassis + " with interface: " + Interface + " and index: " + Index + " MAC:" + Mac);

                                mgmts = lldpctl_atom_get(chassis, lldpctl_k_chassis_mgmt);

                                lldpctl_atom_foreach(mgmts, mgmt) {
                                        std::string SwitchIp(lldpctl_atom_get_str(mgmt, lldpctl_k_mgmt_ip));
                                        std::string WRSwitch("nwt");
                                        if (Chassis.find(WRSwitch) != std::string::npos) {
                                                LogInfo(Chassis + " is a WR Device IP: " + SwitchIp);
                                                try {
                                                        SwitchNode[SwitchIp] = GetNodesInSwitch(SwitchIp);
                                                } catch (const std::exception& e) {
                                                        LogError(std::string("SNMP: ") + e.what());
                                                }
                                        } else {
                                                LogInfo(Chassis + " is NOT a WR Device IP: " + SwitchIp);
                                        }
                                }
                       }
                }
        }

        lldpctl_atom_dec_ref(mgmts);
        lldpctl_atom_dec_ref(neighbors);
        lldpctl_atom_dec_ref(ifaces);

        return 0;
}

void Lldp::ShowSwitch()
{
        for(auto const &ent1 : SwitchNode) {
                LogDebug("WR Switches in Network: " + std::string(ent1.first));
                NodePortMap Nodes = ent1.second;

                for(auto const &ent2 : Nodes)
                        LogDebug("\t|-->in Port: " + std::to_string(ent2.second) + " Node: " + std::string(ent2.first));
        }
}
