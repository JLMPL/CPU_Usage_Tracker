#ifndef MAIN_HEADER_H
#define MAIN_HEADER_H
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_CPU_CORES 64

typedef unsigned long long int ulong;

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

//catcher.c (optional)
void sig_catcher_init(void);

#endif
