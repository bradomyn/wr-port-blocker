#ifndef SSH_CONN_H
#define SSH_CONN_H

#include <iostream>
#include <string.h>
#include <libssh/libssh.h>
#include "Log.h"

class SshConn
{
        public:
                SshConn(LogLevel_t DebugLevel);
                int Config();
                int SendCommand();

        private:
                int VerifyKnownHost(ssh_session session);

                ssh_channel channel;
                ssh_session session;
};
#endif
