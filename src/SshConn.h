#ifndef SSH_CONN_H
#define SSH_CONN_H

#include <iostream>
#include <string.h>
#include <libssh/libssh.h>
#include "Conn.h"
#include "Log.h"

class SshConn : public Conn
{
        public:
                SshConn(int DebugLevel);
                int Config() override;
                int SendCommand(const char* command) override;
                ~SshConn();

        private:
                int VerifyKnownHost(ssh_session session);

                ssh_channel channel ;
                ssh_session session ;
};
#endif
