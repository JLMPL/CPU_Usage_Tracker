#ifndef MAIN_HEADER_H
#define MAIN_HEADER_H

// Hopefully justified
#ifdef __clang__
// doesn't apply since we're using c99+
#pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#endif

#include <pthread.h>
#include <stdatomic.h> //the reason for c11
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <stdnoreturn.h>

// if that's not enough i don't know life anymore
#define MAX_CPU_CORES 128

// typedef unsigned long long int ulong;

// gonna need a bit more than true & false when threads come into play
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
    int num_cores;
}proc_stat_info_t;

typedef struct
{
    int value;
}computed_core_info_t;

typedef struct
{
    computed_core_info_t cpu;
    computed_core_info_t cores[MAX_CPU_CORES];
    int num_cores;
}computed_info_t;

typedef enum
{
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
}logLevel_t;

//main.c
void ct_shutdown(void);

//reader.c
status_t reader_read_proc_stat(proc_stat_info_t* ps_info);

//analyzer.c
status_t analyzer_compute_usage(proc_stat_info_t* ps_info, computed_info_t* c_info);

//printer.c
void printer_print_formatted(computed_info_t* c_info);

//watchdog.c (optional)
// if any thread inactive for 2sec suicide

//logger.c (optional)
// it logs
void logger_log(logLevel_t level, const char* str, ...);

//catcher.c (optional)
void sig_catcher_init(void);

#endif
