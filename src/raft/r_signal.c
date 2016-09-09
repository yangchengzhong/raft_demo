#include "signal.h"

#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

static void _signal_handler(int signo);
typedef struct{
    int  signo;
    char *signame;
    int  flags;
    void (*handler)(int signo);
}_signal_t;

static _signal_t gs_signals[] = 
{
    { SIGSEGV, "SIGSEGV", (int)SA_RESETHAND, _signal_handler },
    { SIGPIPE, "SIGPIPE", 0,                 SIG_IGN },
    { SIGUSR1, "SIGUSR1", 0,                 _signal_handler},
    { 0,        NULL,     0,                 NULL }
};


int r_signal_init()
{
    for(_signal_t *sig = gs_signals; sig->signo != 0; ++sig) 
    {
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = sig->handler;
        sa.sa_flags = sig->flags;
        sigemptyset(&sa.sa_mask);

        int ret = sigaction(sig->signo, &sa, NULL);
        if(0 != ret) 
        {
            printf("sigaction(%s) failed: %s\n", sig->signame, strerror(errno));
            return ret;
        }
    }

    return 0;
}

/*********************** signal ************************/
void _signal_handler(int signo)
{
    _signal_t *sig = NULL;
    for(sig = gs_signals; sig->signo != 0; sig++) 
    {
        if (sig->signo == signo) break;
    }
    if(0 == sig->signo) return ;

    char *actionstr = "";

    switch (signo) 
    {
        case SIGSEGV:
            actionstr = ", core dumping";
            raise(SIGSEGV);
            break;

        case SIGUSR1:
            break;
                
        default:
            break;
    }

    printf("signal %d (%s) received%s ,pid[%d]\n", signo, sig->signame, actionstr, getpid());

    return ;
}
