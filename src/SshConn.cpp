#include "SshConn.h"

SshConn::SshConn(LogLevel_t DebugLevel)
{

        int port = 22;
        session = ssh_new();
        ssh_options_set(session, SSH_OPTIONS_HOST, "localhost");
        ssh_options_set(session, SSH_OPTIONS_PORT, &port);

        if (DebugLevel == LOG_DBG) {
                int verbosity = SSH_LOG_PROTOCOL;
                ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
        }
}

int SshConn::Config()
{
        char *password;
        int rc;

        if (session == NULL){
                LogInfo("creating ssh seassion");
                exit(-1);
        }

        rc = ssh_connect(session);

        LogInfo("Connecting to ssh server");

        if (rc != SSH_OK)
        {
                LogError("ssh connecting"+std::string(ssh_get_error(session)));
                ssh_free(session);
                exit(-1);
        }

        if (VerifyKnownHost(session) < 0)
        {
                ssh_disconnect(session);
                ssh_free(session);
                exit(-1);
        }

        password = getpass("Password: ");
        rc = ssh_userauth_password(session, NULL, password);
        if (rc != SSH_AUTH_SUCCESS)
        {
                LogError("ssh authenticating with password:"+ std::string(ssh_get_error(session)));
                ssh_disconnect(session);
                ssh_free(session);
                exit(-1);
        }

        channel = ssh_channel_new(session);
        if (channel == NULL) {
                LogError("ssh creating a new channel");
                return SSH_ERROR;
        }

        rc = ssh_channel_open_session(channel);
        if (rc != SSH_OK)
        {
                ssh_channel_free(channel);
                LogError("ssh openning new channel");
                return rc;
        }

        return 0;
}

int SshConn::SendCommand()
{
        int rc;
        char buffer[250];
        int nbytes;

        rc = ssh_channel_request_exec(channel, "ls");
        if (rc != SSH_OK)
        {
                ssh_channel_close(channel);
                ssh_channel_free(channel);
                LogError("ssh executing remotly a command");
                return rc;
        }

        LogInfo("GO SSH");

        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);

        while (nbytes > 0)
        {
                if (write(1, buffer, nbytes) != (unsigned int) nbytes)
                {
                        ssh_channel_close(channel);
                        ssh_channel_free(channel);
                        LogError("ssh reading out the channel");
                        return SSH_ERROR;
                }
                nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
        }

        if (nbytes < 0)
        {
                ssh_channel_close(channel);
                ssh_channel_free(channel);
                LogError("ssh reading out the channel");
                return SSH_ERROR;
        }

        return 0;
}

int SshConn::VerifyKnownHost(ssh_session session)
{
        int hlen = 0;
        unsigned char *hash = NULL;
        char *hexa;
        char buf[10];
        int state = ssh_is_server_known(session);

        switch (state)
        {
                case SSH_SERVER_KNOWN_OK:
                        break; /* ok */
                case SSH_SERVER_KNOWN_CHANGED:
                        LogError( "Host key for server changed: it is now:");
                        ssh_print_hexa("Public key hash", hash, hlen);
                        LogError( "For security reasons, connection will be stopped");
                        free(hash);
                        return -1;
                case SSH_SERVER_FOUND_OTHER:
                        LogError( "The host key for this server was not found but an other"
                                        "type of key exists.");
                        LogError( "An attacker might change the default server key to"
                                        "confuse your client into thinking the key does not exist");
                        free(hash);
                        return -1;
                case SSH_SERVER_FILE_NOT_FOUND:
                        LogError( "Could not find known host file.");
                        LogError( "If you accept the host key here, the file will be"
                                        "automatically created.");
                        /* fallback to SSH_SERVER_NOT_KNOWN behavior */
                case SSH_SERVER_NOT_KNOWN:
                        hexa = ssh_get_hexa(hash, hlen);
                        LogError("The server is unknown. Do you trust the host key?");
                        LogError( "Public key hash:" + std::string(hexa));
                        free(hexa);
                        if (fgets(buf, sizeof(buf), stdin) == NULL)
                        {
                                free(hash);
                                return -1;
                        }
                        if (strncasecmp(buf, "yes", 3) != 0)
                        {
                                free(hash);
                                return -1;
                        }
                        if (ssh_write_knownhost(session) < 0)
                        {
                                LogError( "Error " + std::string(strerror(errno)));
                                free(hash);
                                return -1;
                        }
                        break;
                case SSH_SERVER_ERROR:
                        LogError( "Error " +  std::string(ssh_get_error(session)));
                        free(hash);
                        return -1;
        }
        free(hash);
        return 0;
}
