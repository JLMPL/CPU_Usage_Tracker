#include "cutter.h"

void printer_print_formatted(computed_info_t* c_info)
{
    logger_log(LOG_INFO, "pid %d\n", getpid());
    printf("total\t%d\n", c_info->cpu.value);

    for (int i = 0; i < c_info->num_cores; i++)
    {
        printf("cpu%d\t%d\n", i, c_info->cores[i].value);
    }
}

static void* printer_job(void* data)
{
    UNPACK_JOB_ARGS;

    while (true)
    {
        sem_wait(&syncs->c_info_taken_semaphore);
        pthread_mutex_lock(&syncs->c_info_mutex);
            printer_print_formatted(&buffers->c_infos[buffers->num_c_infos]);
            buffers->num_c_infos--;
        pthread_mutex_unlock(&syncs->c_info_mutex);
        sem_post(&syncs->c_info_empty_semaphore);
    }

    return NULL;
}

void printer_job_init(job_t* printer, sync_primitives_t* sync_prims, exchange_buffers_t* buffers)
{
    memset(printer, 0, sizeof(job_t));

    printer->args = (job_arguments_t){
        .prims = sync_prims,
        .buffers = buffers
    };

    pthread_create(&printer->thread, NULL, &printer_job, &printer->args);
}
