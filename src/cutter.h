#ifndef MAIN_HEADER_H
#define MAIN_HEADER_H

// Hopefully justified
#ifdef __clang__
// doesn't apply since we're using c99+
#pragma clang diagnostic ignored "-Wdeclaration-after-statement"

// not supposed to be a literal
#pragma clang diagnostic ignored "-Wformat-nonliteral"

// comes from sigaction.h nothing I can do
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"

// just trying to print a %
// #pragma clang diagnostic ignored "-Wformat-invalid-specifier"

#endif

#include <pthread.h>
#include <stdatomic.h> //the reason for c11 for now
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <stdnoreturn.h>
#include <sys/sysinfo.h>

// if that's not enough i don't know life anymore
#define MAX_CPU_CORES 128
#define NUM_EXCHANGE_BUFFER_ENTRIES 32 //arbitrary

// "Staraj się napisać kod w duchu paradygmatu obiektoweg"
//

typedef enum
{
    CT_FAILURE,
    CT_SUCCESS
}status_t;

typedef struct
{
    int user;
    int nice;
    int system;
    int idle;
    int iowait;
    int irq;
    int softirq;
    int steal;
    int guest;
    int guest_nice;
}proc_stat_core_info_t;

typedef struct
{
    proc_stat_core_info_t cpu;
    proc_stat_core_info_t cores[MAX_CPU_CORES];
}proc_stat_info_t;

typedef struct
{
    float value;
}computed_core_info_t;

typedef struct
{
    computed_core_info_t cpu;
    computed_core_info_t cores[MAX_CPU_CORES];
}computed_info_t;

typedef enum
{
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
}log_level_t;

typedef struct
{
    pthread_mutex_t ps_info_mutex;
    sem_t ps_info_empty_semaphore;
    sem_t ps_info_taken_semaphore;

    pthread_mutex_t c_info_mutex;
    sem_t c_info_empty_semaphore;
    sem_t c_info_taken_semaphore;
}sync_primitives_t;

typedef struct
{
    proc_stat_info_t ps_infos[NUM_EXCHANGE_BUFFER_ENTRIES];
    int num_ps_infos;

    computed_info_t c_infos[NUM_EXCHANGE_BUFFER_ENTRIES];
    int num_c_infos;
}exchange_buffers_t;

typedef struct
{
    sync_primitives_t* prims;
    exchange_buffers_t* buffers;
}job_arguments_t;

typedef struct
{
    job_arguments_t args;
    pthread_t thread;
}job_t;

#define UNPACK_JOB_ARGS job_arguments_t* args = (job_arguments_t*)data;\
    sync_primitives_t* syncs = (sync_primitives_t*)args->prims;\
    exchange_buffers_t* buffers = (exchange_buffers_t*)args->buffers

//main.c
noreturn void ct_shutdown(void);

//threading.c
void job_kill(job_t* job);
void job_wait(job_t* job);

void sync_primitives_init(sync_primitives_t* prims);
void sync_primitives_destroy(sync_primitives_t* prims);

void exchange_buffers_init(exchange_buffers_t* buffs);

//reader.c
void reader_job_init(job_t* reader, sync_primitives_t* sync_prims, exchange_buffers_t* buffers);
status_t reader_read_proc_stat(proc_stat_info_t* ps_info);

//analyzer.c
void analyzer_job_init(job_t* analyzer, sync_primitives_t* sync_prims, exchange_buffers_t* buffers);
status_t analyzer_compute_usage(proc_stat_info_t* ps_info, computed_info_t* c_info);

//printer.c
void printer_job_init(job_t* printer, sync_primitives_t* sync_prims, exchange_buffers_t* buffers);
void printer_print_formatted(computed_info_t* c_info);

//watchdog.c (optional)
// if any thread inactive for 2sec suicide

//logger.c (optional)
// it logs
void logger_job_init(job_t* logger, sync_primitives_t* sync_prims);
void logger_log(log_level_t level, const char* str, ...);
void logger_destroy(void);

//catcher.c (optional)
void sig_catcher_init(void);

#endif
