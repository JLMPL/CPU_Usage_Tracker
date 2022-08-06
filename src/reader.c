#include "cutter.h"
#include <sys/sysinfo.h>

#define PROC_STAT_PATH "/proc/stat"
#define READ_BUFFER_SIZE 256

// #define LOG_READER

static void read_proc_stat_line(const char* str, proc_stat_core_info_t* core)
{
    char junk[8];

    sscanf(str, "%s" "%d %d %d %d %d" "%d %d %d %d %d",
        junk,
        &core->user,
        &core->nice,
        &core->system,
        &core->idle,
        &core->iowait,
        &core->irq,
        &core->softirq,
        &core->steal,
        &core->guest,
        &core->guest_nice
    );

#ifdef LOG_READER
    printf("%d %d %d %d %d" "%d %d %d %d %d\n",
        core->user,
        core->nice,
        core->system,
        core->idle,
        core->iowait,
        core->irq,
        core->softirq,
        core->steal,
        core->guest,
        core->guest_nice
    );
#endif
}

status_t reader_read_proc_stat(proc_stat_info_t* ps_info)
{
    memset(ps_info, 0, sizeof(proc_stat_info_t));

    FILE* file = fopen(PROC_STAT_PATH, "r");

    if (!file)
        return CT_FAILURE;

    ps_info->num_cores = get_nprocs();

    char buffer[READ_BUFFER_SIZE];
    fgets(buffer, READ_BUFFER_SIZE, file);
    read_proc_stat_line(buffer, &ps_info->cpu);

    for (int i = 0; i < ps_info->num_cores; i++)
    {
        fgets(buffer, READ_BUFFER_SIZE, file);
        read_proc_stat_line(buffer, &ps_info->cores[i]);
    }

    fclose(file);

    return CT_SUCCESS;
}

static void* reader_job(void* data)
{
    UNPACK_JOB_ARGS;

    while (true)
    {
        sem_wait(&syncs->ps_info_empty_semaphore);
        pthread_mutex_lock(&syncs->ps_info_mutex);
            reader_read_proc_stat(&buffers->ps_infos[buffers->num_ps_infos]);
            buffers->num_ps_infos++;
            // logger_log(LOG_INFO, "reader_job reading!\n");
        pthread_mutex_unlock(&syncs->ps_info_mutex);
        sem_post(&syncs->ps_info_taken_semaphore);
    }

    return NULL;
}

void reader_job_init(job_t* reader, sync_primitives_t* sync_prims, exchange_buffers_t* buffers)
{
    memset(reader, 0, sizeof(job_t));

    reader->args = (job_arguments_t){
        .prims = sync_prims,
        .buffers = buffers
    };

    pthread_create(&reader->thread, NULL, &reader_job, &reader->args);
}
