#include "Lldp.h"

Lldp::Lldp(int DebugLevel) : Log(DebugLevel)
{

}

Lldp::~Lldp()
{

}

int Lldp::GetNodesInSwitch()
{

        struct snmp_session session, *ss;
        struct snmp_pdu *pdu;
        struct snmp_pdu *response;
        const char *our_v3_passphrase = "The UCD Demo Password";

        oid anOID[MAX_OID_LEN];
        size_t anOID_len = MAX_OID_LEN;

        struct variable_list *vars;
        int status;
        char ip[] = "192.168.20.64";
        unsigned char pub[] = "public";

        init_snmp("snmpapp");

        snmp_sess_init( &session );                   /* set up defaults */
        session.peername = ip;

        /* set the SNMP version number */
        session.version = SNMP_VERSION_1;

        /* set the SNMPv1 community name used for authentication */
        session.community = pub;
        session.community_len = strlen((const char*)session.community);
        //session.community_len = 6;

        /* set the SNMP version number */
        //session.version=SNMP_VERSION_3;

        ///* set the SNMPv3 user name */
        //session.securityName = strdup("root");
        //session.securityNameLen = strlen(session.securityName);

        ///* set the security level to authenticated, but not
        // * encrypted */
        //session.securityLevel = SNMP_SEC_LEVEL_NOAUTH;

        ///* set the authentication method to MD5 */
        //session.securityAuthProto = usmHMACMD5AuthProtocol;
        //session.securityAuthProtoLen = sizeof(usmHMACMD5AuthProtocol)/sizeof(oid);
        //session.securityAuthKeyLen = USM_AUTH_KU_LEN;

        //std::cout << "step 1" << std::endl;

        //if (generate_Ku(session.securityAuthProto,
        //                        session.securityAuthProtoLen,
        //                        (u_char *) our_v3_passphrase, strlen(our_v3_passphrase),
        //                        session.securityAuthKey,
        //                        &session.securityAuthKeyLen) != SNMPERR_SUCCESS) {
        //        snmp_log(LOG_ERR, "Error generating Ku from authentication pass phrase. \n");
        //        return -1;
        //}

        ss = snmp_open(&session);

        std::cout << "step 2" << std::endl;

        if (!ss) {
                snmp_perror("ack");
                snmp_log(LOG_ERR, "something horrible happened!!!\n");
                return -1;
        }

        std::cout << "step 3" << std::endl;

        pdu = snmp_pdu_create(SNMP_MSG_GET);

        anOID_len = MAX_OID_LEN;

        //if (!snmp_parse_oid("iso.0.8802.1.1.2.1.4.1.1.10.33462800.2.8", anOID, &anOID_len)) {
        if (!snmp_parse_oid(".1.3.6.1.2.1.1.1.0", anOID, &anOID_len)) {
                snmp_perror(".1.3.6.1.2.1.1.1.0");
                return -1;
        }

        //read_objid(".1.3.6.1.2.1.1.1.0", anOID, &anOID_len);
        //read_objid("iso.0.8802.1.1.2.1.4.1.1.10.33462800.2.8", anOID, &anOID_len);


        snmp_add_null_var(pdu, anOID, anOID_len);
        status = snmp_synch_response(ss, pdu, &response);

        if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
                for(vars = response->variables; vars; vars = vars->next_variable)
                        print_variable(vars->name, vars->name_length, vars);

                std::cout << "step 4" << std::endl;

                for(vars = response->variables; vars; vars = vars->next_variable) {
                        int count=1;
                        if (vars->type == ASN_OCTET_STR) {
                                char *sp = (char *)malloc(1 + vars->val_len);
                                memcpy(sp, vars->val.string, vars->val_len);
                                sp[vars->val_len] = '\0';
                                printf("value #%d is a string: %s\n", count++, sp);
                                free(sp);
                        }
                        else
                                printf("value #%d is NOT a string! Ack!\n", count++);
                }
        } else {
                if (status == STAT_SUCCESS)
                        fprintf(stderr, "Error in packet\nReason: %s\n",
                                        snmp_errstring(response->errstat));
                else
                        snmp_sess_perror("snmpget", ss);
        }

        if (response)
                snmp_free_pdu(response);
        snmp_close(ss);


        //struct snmp_session session;
        //struct snmp_session *sess_handle;

        //struct snmp_pdu *pdu;
        //struct snmp_pdu *response;

        //struct variable_list *vars;

        //unsigned char pub[7] = "public";
        //char ip[15] = "192.168.20.64";

        //oid id_oid[MAX_OID_LEN];
        //oid serial_oid[MAX_OID_LEN];

        //size_t id_len = MAX_OID_LEN;
        //size_t serial_len = MAX_OID_LEN;

        //int status;

        //struct tree * mib_tree;

        ///*********************/

        //init_snmp("APC Check");

        //snmp_sess_init( &session );
        //session.version = SNMP_VERSION_1;
        //session.community = pub;
        //session.community_len = 7;
        //session.peername = ip;
        //sess_handle = snmp_open(&session);

        //add_mibdir(".");
        //mib_tree = read_mib("PowerNet-MIB.txt");

        //pdu = snmp_pdu_create(SNMP_MSG_GET);

        //read_objid("PowerNet-MIB::upsBasicIdentModel.0", id_oid, &id_len);
        //snmp_add_null_var(pdu, id_oid, id_len);
        //read_objid("PowerNet-MIB::upsAdvIdentSerialNumber.0", serial_oid, &serial_len);
        //snmp_add_null_var(pdu, serial_oid, serial_len);

        //status = snmp_synch_response(sess_handle, pdu, &response);

        //for(vars = response->variables; vars; vars = vars->next_variable) {
        //        print_value(vars->name, vars->name_length, vars);
        //        std::cout << "snmp" << vars;
        //}

        //snmp_free_pdu(response);
        //snmp_close(sess_handle);

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
