#define _GNU_SOURCE
#include <time.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "devious_log.h"

int setsockopt(int sockfd, int level, int optname,
    const void *optval, socklen_t optlen)
{
    int (*real)(int sockfd, int level, int optname,
        const void *optval, socklen_t optlen)
        = dlsym(RTLD_NEXT, "setsockopt");
    int one = 1;

    if (level == SOL_IPV6 && optname == IPV6_V6ONLY &&
        optlen == 4 && optval && *(int*)(optval) == 0)
    {
        devious_log("You shall not accept IPv4!\n");
        optval = &one;
    }
    return real(sockfd, level, optname, optval, optlen);
}
