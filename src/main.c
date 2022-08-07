#include "cutter.h"

static sync_primitives_t  sync_prims;
static exchange_buffers_t buffers;

static job_t reader;
static job_t analyzer;
static job_t printer;
static job_t logger;

void ct_shutdown(void)
{
    job_kill(&reader);
    job_kill(&analyzer);
    job_kill(&printer);
    job_kill(&logger);

    logger_destroy();
    sync_primitives_destroy(&sync_prims);

    logger_log(LOG_INFO, "Beautiful death\n");
    exit(0);
}

int main(void)
{
    sig_catcher_init();
    sync_primitives_init(&sync_prims);
    exchange_buffers_init(&buffers);

    logger_job_init(&logger, &sync_prims);
    reader_job_init(&reader, &sync_prims, &buffers);
    analyzer_job_init(&analyzer, &sync_prims, &buffers);
    printer_job_init(&printer, &sync_prims, &buffers);

    job_wait(&reader);
    job_wait(&analyzer);
    job_wait(&printer);
    job_wait(&logger);

    ct_shutdown();
}

/*

TODO:
do a test (make test)
watchdog
logger -> file

*/
