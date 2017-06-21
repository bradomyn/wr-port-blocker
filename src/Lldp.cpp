#include "Lldp.h"

#define NETSNMP_DS_WALK_INCLUDE_REQUESTED	        1
#define NETSNMP_DS_WALK_PRINT_STATISTICS	        2
#define NETSNMP_DS_WALK_DONT_CHECK_LEXICOGRAPHIC	3
#define NETSNMP_DS_WALK_TIME_RESULTS     	        4
#define NETSNMP_DS_WALK_DONT_GET_REQUESTED	        5
#define NETSNMP_DS_WALK_TIME_RESULTS_SINGLE	        6

Lldp::Lldp(int DebugLevel) : Log(DebugLevel)
{

}

Lldp::~Lldp()
{

}

void Lldp::snmp_get_and_print(netsnmp_session * ss, oid * theoid, size_t theoid_len, int pdu_t)
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
            print_variable(vars->name, vars->name_length, vars);
        }
    }
    if (response) {
        snmp_free_pdu(response);
    }
    std::cout << "step 2" << std::endl;
}


int Lldp::GetNodesInSwitch()
{

        netsnmp_session session;
        netsnmp_session *ss;
        netsnmp_pdu *pdu;
        netsnmp_pdu *response;
        //const char *our_v3_passphrase = "The UCD Demo Password";

        oid anOID[MAX_OID_LEN];
        oid end_OID[MAX_OID_LEN];
        oid next_OID[MAX_OID_LEN];
        size_t anOID_len = MAX_OID_LEN;
        size_t end_len;
        size_t next_len;
        int count;
        struct timeval  tv_a, tv_b;

        struct variable_list *vars;
        int status;
        int check;
        int running;
        char ip[] = "192.168.20.64";
        unsigned char pub[] = "public";

        init_snmp("snmpapp");

        snmp_sess_init( &session );                   /* set up defaults */
        session.peername = ip;

        /* set the SNMP version number */
        session.version = SNMP_VERSION_2c;

        /* set the SNMPv2c community name used for authentication */
        session.community = pub;
        session.community_len = strlen((const char*)session.community);

        std::cout << "step 1" << std::endl;


        anOID_len = MAX_OID_LEN;

        if (!snmp_parse_oid("iso.0.8802.1.1.2.1.4.1.1.5", anOID, &anOID_len)) {
                snmp_perror(".1.3.6.1.2.1.1.1.0");
                return -1;
        }

        memmove(next_OID, anOID, anOID_len * sizeof(oid));
        next_len = anOID_len;


        memmove(end_OID, anOID, anOID_len * sizeof(oid));
        end_len = anOID_len;
        end_OID[end_len-1]++;


        ss = snmp_open(&session);

        if (ss == NULL) {
                snmp_perror("ack");
                snmp_log(LOG_ERR, "something horrible happened!!!\n");
                return -1;
        }

        check =
        !netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID,
                        NETSNMP_DS_WALK_DONT_CHECK_LEXICOGRAPHIC);

        snmp_get_and_print(ss, anOID, anOID_len, SNMP_MSG_GET);

        //if (netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_WALK_INCLUDE_REQUESTED)) {
        //        snmp_get_and_print(ss, anOID, anOID_len);
        //}


        running = 1;

        while(running) {
                pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
                snmp_add_null_var(pdu, next_OID, next_len);

                /*
                 * do the request
                 */
                if (netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_WALK_TIME_RESULTS_SINGLE))
                    netsnmp_get_monotonic_clock(&tv_a);
                status = snmp_synch_response(ss, pdu, &response);
                if (status == STAT_SUCCESS) {
                    if (netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_WALK_TIME_RESULTS_SINGLE))
                        netsnmp_get_monotonic_clock(&tv_b);
                    if (response->errstat == SNMP_ERR_NOERROR) {
                        /*
                         * check resulting variables
                         */
                        for (vars = response->variables; vars;
                             vars = vars->next_variable) {
                            if (snmp_oid_compare(end_OID, end_len,
                                                 vars->name, vars->name_length) <= 0) {
                                /*
                                 * not part of this subtree
                                 */
                                running = 0;
                                continue;
                            }
                            numprinted++;
                            if (netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_WALK_TIME_RESULTS_SINGLE))
                                fprintf(stdout, "%f s: ",
                                        (double) (tv_b.tv_usec - tv_a.tv_usec)/1000000 +
                                        (double) (tv_b.tv_sec - tv_a.tv_sec));
                            print_variable(vars->name, vars->name_length, vars);
                            if ((vars->type != SNMP_ENDOFMIBVIEW) &&
                                (vars->type != SNMP_NOSUCHOBJECT) &&
                                (vars->type != SNMP_NOSUCHINSTANCE)) {
                                /*
                                 * not an exception value
                                 */
                                if (check
                                    && snmp_oid_compare(next_OID, next_len,
                                                        vars->name,
                                                        vars->name_length) >= 0) {
                                    fprintf(stderr, "Error: OID not increasing: ");
                                    fprint_objid(stderr, next_OID, next_len);
                                    fprintf(stderr, " >= ");
                                    fprint_objid(stderr, vars->name,
                                                 vars->name_length);
                                    fprintf(stderr, "\n");
                                    running = 0;
                                }
                                memmove((char *) next_OID, (char *) vars->name,
                                        vars->name_length * sizeof(oid));
                                next_len = vars->name_length;
                            } else
                                /*
                                 * an exception value, so stop
                                 */
                                running = 0;
                        }
                    } else {
                        /*
                         * error in response, print it
                         */
                        running = 0;
                        if (response->errstat == SNMP_ERR_NOSUCHNAME) {
                            printf("End of MIB\n");
                        } else {
                            fprintf(stderr, "Error in packet.\nReason: %s\n",
                                    snmp_errstring(response->errstat));
                            if (response->errindex != 0) {
                                fprintf(stderr, "Failed object: ");
                                for (count = 1, vars = response->variables;
                                     vars && count != response->errindex;
                                     vars = vars->next_variable, count++)
                                    /*EMPTY*/;
                                if (vars)
                                    fprint_objid(stderr, vars->name,
                                                 vars->name_length);
                                fprintf(stderr, "\n");
                            }
                        }
                    }
                } else if (status == STAT_TIMEOUT) {
                    fprintf(stderr, "Timeout: No Response from %s\n",
                            session.peername);
                    running = 0;
                } else {                /* status == STAT_ERROR */
                    snmp_sess_perror("snmpwapp", ss);
                    running = 0;
                }
                if (response)
                    snmp_free_pdu(response);
        }

        snmp_close(ss);

        return 0;
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
        std::time_t Ts = std::time(nullptr);

        ctlname = lldpctl_get_default_transport();
        conn = lldpctl_new_name(ctlname, NULL, NULL, user_data);

        if (conn == NULL) {
                LogError("LLDP: Fail to get start liblldp and connect to lldpd daemon");
                return -1;
        }

        LogInfo("LLDP: Started liblldp and connected to lldpd daemon");

        //lldpctl_atom_t *chassis = lldpctl_get_local_chassis(conn);
        lldpctl_atom_t *iface, *ifaces;

        ifaces = lldpctl_get_interfaces(conn);

        if (ifaces == NULL)
                std::cout << "LLDP: NULL iface list" << std::endl;

        lldpctl_atom_foreach(ifaces, iface) {
                port = lldpctl_get_port(iface);
                neighbors = lldpctl_atom_get(port, lldpctl_k_port_neighbors);
                lldpctl_atom_foreach(neighbors, neighbor) {
                        lldpctl_atom_t *chassis = lldpctl_atom_get(neighbor, lldpctl_k_port_chassis);
                        port_num =  lldpctl_atom_get_str(chassis, lldpctl_k_chassis_index);
                        if (port_num != NULL) {
                                //std::cout << lldpctl_atom_get_str(iface, lldpctl_k_interface_name) << std::endl;
                                //std::cout << lldpctl_atom_get_str(chassis, lldpctl_k_chassis_index) << std::endl;
                                //std::cout << lldpctl_atom_get_str(chassis, lldpctl_k_chassis_name) << std::endl;
                                mgmts = lldpctl_atom_get(chassis, lldpctl_k_chassis_mgmt);
                                lldpctl_atom_foreach(mgmts, mgmt) {
                                        //std::cout << lldpctl_atom_get_str(mgmt, lldpctl_k_mgmt_ip) << std::endl;
                                        std::localtime(&Ts);
                                        SwitchList[lldpctl_atom_get_str(mgmt, lldpctl_k_mgmt_ip)] = Ts;
                                }
                       }
                }
        }
        lldpctl_atom_dec_ref(mgmts);
        lldpctl_atom_dec_ref(neighbors);
        lldpctl_atom_dec_ref(ifaces);

        GetNodesInSwitch();

        return 0;
}

void Lldp::ShowSwitch()
{
        for(std::map<std::string, std::time_t>::iterator it = SwitchList.begin(); it != SwitchList.end(); ++it)
                LogInfo("WR Switches in Network: " + std::string(it->first));
}
