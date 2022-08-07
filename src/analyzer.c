#include "cutter.h"

// based on htop source
static float compute_for_core(proc_stat_core_info_t* core_info)
{
    core_info->user = core_info->user - core_info->guest;
    core_info->nice     = core_info->nice - core_info->guest_nice;

    int idlealltime   = core_info->idle + core_info->iowait;
    int systemalltime = core_info->system + core_info->irq + core_info->softirq;
    int virtalltime   = core_info->guest + core_info->guest_nice;

    int total = core_info->user + core_info->nice + systemalltime + idlealltime + core_info->steal + virtalltime;

    float busy_time = (float)total - (float)idlealltime;
    float busy_perc = busy_time / (float)total;

    return busy_perc * 100.f;
}

status_t analyzer_compute_usage(proc_stat_info_t* ps_info, computed_info_t* c_info)
{
    memset(c_info, 0, sizeof(computed_info_t));

    c_info->cpu.value = compute_for_core(&ps_info->cpu);
    c_info->num_cores = ps_info->num_cores;

    for (int i = 0; i < ps_info->num_cores; i++)
    {
        c_info->cores[i].value = compute_for_core(&ps_info->cores[i]);
    }

    return CT_SUCCESS;
}

static void* analyzer_job(void* data)
{
    UNPACK_JOB_ARGS;

    while (true)
    {
        sem_wait(&syncs->ps_info_taken_semaphore);
        sem_wait(&syncs->c_info_empty_semaphore);
        pthread_mutex_lock(&syncs->ps_info_mutex);
        pthread_mutex_lock(&syncs->c_info_mutex);
            analyzer_compute_usage(&buffers->ps_infos[buffers->num_ps_infos - 1], &buffers->c_infos[buffers->num_c_infos]);
            buffers->num_ps_infos--;
            buffers->num_c_infos++;
            // logger_log(LOG_INFO, "analyzer_job running!\n");
        pthread_mutex_unlock(&syncs->ps_info_mutex);
        pthread_mutex_unlock(&syncs->c_info_mutex);
        sem_post(&syncs->ps_info_empty_semaphore);
        sem_post(&syncs->c_info_taken_semaphore);
    }

    return NULL;
}

void analyzer_job_init(job_t* analyzer, sync_primitives_t* sync_prims, exchange_buffers_t* buffers)
{
    memset(analyzer, 0, sizeof(job_t));

    analyzer->args = (job_arguments_t){
        .prims = sync_prims,
        .buffers = buffers
    };

    pthread_create(&analyzer->thread, NULL, &analyzer_job, &analyzer->args);
}
