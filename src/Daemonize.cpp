#include "Daemonize.h"

void Daemonize()
{
    pid_t pid;
    Log Log(LOG_MSG);

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0) {
        Log.LogError("Daemonize: PID Error");
        exit(EXIT_FAILURE);
    }

    /* Success: Let the parent terminate */
    if (pid > 0) {
        Log.LogInfo("Daemonize: PID Created");
        exit(EXIT_SUCCESS);
    }

    /* On success: The child process becomes session leader */
    if (setsid() < 0) {
        Log.LogError("Daemonize: SETSID Error");
        exit(EXIT_FAILURE);
    }

    /* Catch, ignore and handle signals */
    //TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0) {
        Log.LogError("Daemonize: SETSID Error");
        exit(EXIT_FAILURE);
    }

    /* Success: Let the parent terminate */
    if (pid > 0) {
        Log.LogInfo("Daemonize: Parent terminate - PID Created");
        exit(EXIT_SUCCESS);
    }

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/");

    /* Close all open file descriptors */
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
        close (x);
    }
}
