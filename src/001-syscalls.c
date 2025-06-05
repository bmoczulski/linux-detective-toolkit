#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <stddef.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int die(const char *msg)
{
    perror(msg);
    exit(1);
}

int copy_file(int from, int to)
{
    struct stat st;
    if (-1 == fstat(from, &st)) {
        die("can't fstat()");
    }

    int result = sendfile(to, from, NULL, st.st_size);

    if (-1 == result && errno == EINVAL) {
        fprintf(stderr, "WARN: can't sendfile() this descriptor, falling back to manual copy\n");
        goto manually;
    }
    if (result != st.st_size) {
        die("can't sendfile() everything");
    }
    return result;

manually:
    char buf[16];
    ssize_t total = 0;
    do
    {
        ssize_t r = read(from, buf, sizeof(buf));
        if (r < 0) {
            die("can't read()");
        }
        total += r;
        while (r > 0) {
            ssize_t w = write(to, buf, r);
            if (w < 0) {
                die("can't write()");
            }
            r -= w;
        }
    }
    while (total < st.st_size);
    return total;
}

int main()
{
    int fd = -1;
    do
    {
        fd = open("secret.file", O_RDONLY);
        if (-1 == fd)
        {
            sleep(1);
        }
    } while (fd == -1);

    copy_file(fd, 1);
}