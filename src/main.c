#include "cutter.h"
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define NUM_EXCHANGE_BUFFER_ENTRIES 32 //arbitrary

static pthread_t reader_thread;
static pthread_t analyzer_thread;
static pthread_t printer_thread;

static pthread_mutex_t ps_info_mutex;
static sem_t ps_info_empty_semaphore;
static sem_t ps_info_taken_semaphore;
static proc_stat_info_t ps_infos[NUM_EXCHANGE_BUFFER_ENTRIES];
static int num_ps_infos = 0;

static pthread_mutex_t c_info_mutex;
static sem_t c_info_empty_semaphore;
static sem_t c_info_taken_semaphore;
static computed_info_t c_infos[NUM_EXCHANGE_BUFFER_ENTRIES];
static int num_c_infos = 0;

static void* reader_job()
{
    while (true)
    {
        sem_wait(&ps_info_empty_semaphore);
        pthread_mutex_lock(&ps_info_mutex);
            reader_read_proc_stat(&ps_infos[num_ps_infos]);
            num_ps_infos++;
            // logger_log(LOG_INFO, "reader_job reading!\n");
        pthread_mutex_unlock(&ps_info_mutex);
        sem_post(&ps_info_taken_semaphore);
    }

    return NULL;
}

static void* analyzer_job()
{
    while (true)
    {

        sem_wait(&ps_info_taken_semaphore);
        sem_wait(&c_info_empty_semaphore);
        pthread_mutex_lock(&ps_info_mutex);
        pthread_mutex_lock(&c_info_mutex);
            analyzer_compute_usage(&ps_infos[num_ps_infos-1], &c_infos[num_c_infos]);
            num_ps_infos--;
            num_c_infos++;
            // logger_log(LOG_INFO, "analyzer_job running!\n");
        pthread_mutex_unlock(&ps_info_mutex);
        pthread_mutex_unlock(&c_info_mutex);
        sem_post(&ps_info_empty_semaphore);
        sem_post(&c_info_taken_semaphore);


    }

    return NULL;
}

static void* printer_job()
{
    while (true)
    {
        sem_wait(&c_info_taken_semaphore);
        pthread_mutex_lock(&c_info_mutex);
            printer_print_formatted(&c_infos[num_c_infos]);
            num_c_infos--;
        pthread_mutex_unlock(&c_info_mutex);
        sem_post(&c_info_empty_semaphore);
    }

    return NULL;
}

int main(void)
{
    sig_catcher_init();

    pthread_mutex_init(&ps_info_mutex, NULL);
    sem_init(&ps_info_empty_semaphore, 0, NUM_EXCHANGE_BUFFER_ENTRIES);
    sem_init(&ps_info_taken_semaphore, 0, 0);

    pthread_mutex_init(&c_info_mutex, NULL);
    sem_init(&c_info_empty_semaphore, 0, NUM_EXCHANGE_BUFFER_ENTRIES);
    sem_init(&c_info_taken_semaphore, 0, 0);

    pthread_create(&reader_thread, NULL, &reader_job, NULL);
    pthread_create(&analyzer_thread, NULL, &analyzer_job, NULL);
    pthread_create(&printer_thread, NULL, &printer_job, NULL);

    pthread_join(reader_thread, NULL);
    pthread_join(analyzer_thread, NULL);
    pthread_join(printer_thread, NULL);

    pthread_mutex_destroy(&ps_info_mutex);
    sem_destroy(&ps_info_empty_semaphore);
    sem_destroy(&ps_info_taken_semaphore);

    pthread_mutex_destroy(&c_info_mutex);
    sem_destroy(&c_info_empty_semaphore);
    sem_destroy(&c_info_taken_semaphore);

    logger_log(LOG_INFO, "program closed correctly\n");
    return 0;
}
