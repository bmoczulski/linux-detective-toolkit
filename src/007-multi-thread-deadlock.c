#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t *mutexes = NULL;
pthread_mutex_t *threads = NULL;

int steps = 100;
int amount = 1;
int thread_count = 1;
int mutex_count = 1;
int total = 0;

void die(int error, const char *msg)
{
    fprintf(stderr, "error = %d, %s\n", error, strerror(error));
    perror(msg);
    exit(1);
}

void lock_all_starting_with(int nth)
{
    for (int i = 0; i < mutex_count; ++i)
    {
        int m_index = (i + nth) % mutex_count;
        int r = pthread_mutex_lock(&mutexes[m_index]);
        if (r != 0) {
            die(r, "can't lock mutex");
        }
    }
}

void unlock_all_starting_with(int nth)
{
    for (int i = 0; i < mutex_count; ++i)
    {
        int m_index = (i + nth) % mutex_count;
        int r = pthread_mutex_unlock(&mutexes[m_index]);
        if (r != 0) {
            die(r, "can't lock mutex");
        }
    }
}


void* thread(void *)
{
    for (int i = 0; i < steps; ++i)
    {
        lock_all_starting_with(rand());
        total += amount;
        unlock_all_starting_with(rand());
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    if (argc > 1) {
        steps = strtol(argv[1], NULL, 0);
    }
    if (argc > 2) {
        amount = strtol(argv[2], NULL, 0);
    }
    if (argc > 3) {
        mutex_count = strtol(argv[3], NULL, 0);
    }
    if (argc > 4) {
        thread_count = strtol(argv[4], NULL, 0);
    }

    mutexes = calloc(mutex_count, sizeof(*mutexes));
    for (int i = 0; i < mutex_count; ++i)
    {
        pthread_mutex_init(&mutexes[i], NULL);
    }

    pthread_t *threads = calloc(thread_count, sizeof(*threads));
    for (int i = 0; i < thread_count; ++i)
    {
        pthread_create(&threads[i], NULL, thread, NULL);
    }

    for (int i = 0; i < thread_count; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    printf("total = %d\n", total);
}