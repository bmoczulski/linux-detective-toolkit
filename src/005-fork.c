/*
 *  Example from "Linux Detective Toolkit: 360° debugging and black-box analysis without printf() or GDB" by Bartosz Moczulski
 *  Copyright © 2025
 *
 *  Provided for educational purposes only.
 *  Not for reuse, redistribution, or public presentation without permission.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int die(const char *msg)
{
    perror(msg);
    exit(1);
}


int main()
{
    int pid = fork();
    if (pid < 0) {
        die("Can't fork()");
    }
    else if (pid == 0) {
        for (;;) {
            printf("I am the forked process, pid = %d, parent pid = %d\n", getpid(), getppid());
            sleep(1);
        }
    }
    else {
        printf("I am the parent process, pid = %d, child pid = %d\n", getpid(), pid);
        sleep(1);
    }
}