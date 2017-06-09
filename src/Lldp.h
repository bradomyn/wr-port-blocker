#ifndef LLDP_H
#define LLDP_H

#include <lldpctl.h>
#include <stdint.h>
#include "Log.h"

class Lldp : public Log
{
        public:
                Lldp(int DebugLevel);
                ~Lldp();
                int GetLldp(void);
                //lldpctl_send_callback send(lldpctl_conn_t *conn, const uint8_t *data, size_t length, void *user_data);
};
#endif
