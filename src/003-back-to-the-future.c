#define _GNU_SOURCE
#include <time.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include "devious_log.h"

int clock_gettime(clockid_t clockid, struct timespec *tp)
{
    int (*real)(clockid_t clockid, struct timespec *tp) = dlsym(RTLD_NEXT, "clock_gettime");
    int result = real(clockid, tp);
    if (result == 0) {
        int offset = 3600 * 24 * 365;
        const char *offset_s = getenv("CLOCK_OFFSET");
        if (offset_s) {
            offset = strtol(offset_s, NULL, 0);
            devious_log("LD_PRELOAD: clock_gettime() called, offset = %ds\n", offset);
        }
        else {
            devious_log("LD_PRELOAD: clock_gettime() called, offset = +1y\n");
        }
        tp->tv_sec += offset;
    }
    return result;
}