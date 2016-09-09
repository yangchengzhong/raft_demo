#include "log.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>


void log_with_time(int level, const char *fmt, ...)
{
    va_list args;
    
    char date[32] = {0};
    struct timeval tv;
    
    /* print the progname, version, and timestamp */
    gettimeofday(&tv, NULL);
    strftime(date, 32, "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));
    
    char *buf = NULL;
    /* printf like normal */
    va_start(args, fmt);
    //vprintf(fmt, args);
    vasprintf(&buf, fmt, args);
    va_end(args);
    
    printf("[%s.%03d]%s\n", date, (int)tv.tv_usec / 1000, buf);

    free(buf);
}

