#define _GNU_SOURCE
#include <time.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include "devious_log.h"

const char * maybe_override(const char *path)
{
    if (strcmp(path, "secret.file") == 0 || strcmp(path, "./secret.file") == 0) {
        path = getenv("SECRET_FILE");
        if (!path) {
            path = "/etc/timezone";
        }
        devious_log("You want the \"secret.file\"? Have \"%s\" instead!\n", path);
    }
    else {
        devious_log("open(\"%s\") - no changes\n", path);
    }
    return path;
}

int open(const char *path, int flags, ...)
{
    int (*real)(const char *path, int flags, ...)
        = dlsym(RTLD_NEXT, "open");

    path = maybe_override(path);

    int result = 0;
    if ((flags & O_CREAT) != 0) {
        va_list ap;
        va_start(ap, flags);
        int mode = va_arg(ap, int);
        va_end(ap);

        result = real(path, flags, mode);
    }
    else {
        result = real(path, flags);
    }
    return result;
}
