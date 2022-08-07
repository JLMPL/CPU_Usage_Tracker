#include "cutter.h"

void watchdog_report(watchdog_target_t who)
{

}

static void* watchdog_job()
{
    while (true)
    {

    }

    return NULL;
}

void watchdog_job_init(job_t* watchdog)
{
    memset(watchdog, 0, sizeof(job_t));

    pthread_create(&watchdog->thread, NULL, &watchdog_job, NULL);
}
