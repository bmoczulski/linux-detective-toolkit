#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mA;
pthread_mutex_t mB;

int steps = 100;
int amount = 1;
int total = 0;

void die(int error, const char *msg)
{
    fprintf(stderr, "error = %d, %s\n", error, strerror(error));
    perror(msg);
    exit(1);
}


void* thread_AB(void *)
{
    for (int i = 0; i < steps; ++i)
    {
        int r;

        r = pthread_mutex_lock(&mA);
        if (r != 0) {
            die(r, "can't lock mutex 1");
        }

        r = pthread_mutex_lock(&mB);
        if (r != 0) {
            die(r, "can't lock mutex 2");
        }

        total += amount;

        r = pthread_mutex_unlock(&mB);
        if (r != 0) {
            die(r, "can't unlock mutex 2");
        }

        r = pthread_mutex_unlock(&mA);
        if (r != 0) {
            die(r, "can't unlock mutex 1");
        }
    }
    return NULL;
}

void* thread_BA(void *)
{
    for (int i = 0; i < steps; ++i)
    {
        int r;

        r = pthread_mutex_lock(&mB);
        if (r != 0) {
            die(r, "can't lock mutex 2");
        }

        r = pthread_mutex_lock(&mA);
        if (r != 0) {
            die(r, "can't lock mutex 1");
        }

        total += amount;

        r = pthread_mutex_unlock(&mA);
        if (r != 0) {
            die(r, "can't unlock mutex 1");
        }

        r = pthread_mutex_unlock(&mB);
        if (r != 0) {
            die(r, "can't unlock mutex 2");
        }
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    pthread_t tAB;
    pthread_t tBA;

    if (argc > 1) {
        steps = strtol(argv[1], NULL, 0);
    }
    if (argc > 2) {
        amount = strtol(argv[2], NULL, 0);
    }

    pthread_mutex_init(&mA, NULL);
    pthread_mutex_init(&mB, NULL);

    pthread_create(&tAB, NULL, thread_AB, NULL);
    pthread_create(&tBA, NULL, thread_BA, NULL);

    pthread_join(tAB, NULL);
    pthread_join(tBA, NULL);

    printf("total = %d\n", total);
}