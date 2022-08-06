#include "cutter.h"

void job_wait(job_t* job)
{
    pthread_join(job->thread, NULL);
}

void job_kill(job_t* job)
{
    pthread_cancel(job->thread);
}

void sync_primitives_init(sync_primitives_t* prims)
{
    pthread_mutex_init(&prims->ps_info_mutex, NULL);
    sem_init(&prims->ps_info_empty_semaphore, 0, NUM_EXCHANGE_BUFFER_ENTRIES);
    sem_init(&prims->ps_info_taken_semaphore, 0, 0);

    pthread_mutex_init(&prims->c_info_mutex, NULL);
    sem_init(&prims->c_info_empty_semaphore, 0, NUM_EXCHANGE_BUFFER_ENTRIES);
    sem_init(&prims->c_info_taken_semaphore, 0, 0);
}

void sync_primitives_destroy(sync_primitives_t* prims)
{
    pthread_mutex_destroy(&prims->ps_info_mutex);
    sem_destroy(&prims->ps_info_empty_semaphore);
    sem_destroy(&prims->ps_info_taken_semaphore);

    pthread_mutex_destroy(&prims->c_info_mutex);
    sem_destroy(&prims->c_info_empty_semaphore);
    sem_destroy(&prims->c_info_taken_semaphore);
}

void exchange_buffers_init(exchange_buffers_t* buffs)
{
    memset(buffs, 0, sizeof(exchange_buffers_t));
}
