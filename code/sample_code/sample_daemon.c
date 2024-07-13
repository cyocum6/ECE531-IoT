// sample code
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <stdbool.h>
//#include <error_format.h>     // gets error for some reason, removed ERROR_FORMAT below

#define OK              0
#define ERR_SETSID      1
#define SIGTERM         2
#define SIGHUP          3
#define ERR_FORK        4
#define ERR_CHDIR       5
#define ERR_WTF         9
#define DAEMON_NAME     "Sample_daemon"


//Signal Handling:
static void _signal_handler(const int signal) {
	switch(signal) {
        case SIGHUP:
            break;
        case SIGTERM:
            syslog(LOG_INFO, "received SIGTERM, exiting.");
            closelog();
            exit(OK);
            break;
        default:
            syslog(LOG_INFO, "received unhandled signal");
}
}

static void _do_work(void) {
	for(int i = 0; true; i++) {
		syslog(LOG_INFO, "iteration: %d", i);
		sleep(1);		// daemon does work, then sleeps
	}
}






int main(void) {
    
    openlog(DAEMON_NAME, LOG_PID | LOG_NDELAY | LOG_NOWAIT, LOG_DAEMON);
    syslog(LOG_INFO, "starting sampled");	// naming file with end “d” tells it’s a daemon

    // avoids taking over syslog by implementing a fork
    pid_t pid = fork();		
    if (pid < 0) {
        syslog(LOG_ERR, strerror(errno));
        return ERR_FORK;	// if something went wrong with fork, notifies log and exit
    }			        // equates to IF (PID < 0) EXIT (1) after logging

    // received PID if we’re parent process, now exiting (only care about forked child)
    if (pid > 0) {
        return OK;		// equates to IF (PID > 0) EXIT (0)
    }

    // create/set session
    if (setsid() < -1) {
        syslog(LOG_ERR, strerror(errno));
        return ERR_SETSID;	// demands to be leader of session
    }

    // close file pointers
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);	

    umask(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);	// used for when creating files…

    // changes to working directory as root for easier navigation if needed
    if (chkir("/") < 0) {
        syslog(LOG_ERR, strerror(errno));
        return ERR_CHDIR;	
    }

    // now only controlled by signals, no interactive user session
    signal(SIGTERM, _signal_handler);
    signal(SIGHUP, _signal_handler);	

    _do_work();			// daemon process runloop, perhaps while loop execute responsibilities, log here
    return ERR_WTF;		// should never be reached; s/b looping in _do_work until daemon killed-> exit

}