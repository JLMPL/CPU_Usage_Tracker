#include "cutter.h"
#include <unistd.h>

static pthread_t reader_thread;
static pthread_t analyzer_thread;
static pthread_t printer_thread;

static void* reader_job()
{
    while (true)
    {
        proc_stat_info_t info;
        reader_read_proc_stat(&info);

        logger_log(LOG_INFO, "reader_job running!\n");
    }

    return NULL;
}

static void* analyzer_job()
{
    while (true)
    {
        proc_stat_info_t info;
        computed_info_t c_info;
        analyzer_compute_usage(&info, &c_info);

        logger_log(LOG_INFO, "analyzer_job running!\n");
    }

    return NULL;
}

static void* printer_job()
{
    while (true)
    {
        // computed_info_t c_info;
        // printer_print_formatted(&c_info);

        logger_log(LOG_INFO, "printer_job running!\n");
    }

    return NULL;
}

int main(void)
{
    sig_catcher_init();

    pthread_create(&reader_thread, NULL, &reader_job, NULL);
    pthread_create(&analyzer_thread, NULL, &analyzer_job, NULL);
    pthread_create(&printer_thread, NULL, &printer_job, NULL);

    pthread_join(reader_thread, NULL);
    pthread_join(analyzer_thread, NULL);
    pthread_join(printer_thread, NULL);

    return 0;
}
