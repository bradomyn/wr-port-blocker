#include "Lldp.h"

Lldp::Lldp(int DebugLevel) : Log(DebugLevel)
{

}

Lldp::~Lldp()
{

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

        return 0;
}

void Lldp::ShowSwitch()
{
        for(std::map<std::string, std::time_t>::iterator it = SwitchList.begin(); it != SwitchList.end(); ++it)
                LogInfo("WR Switches in Network: " + std::string(it->first));
}
