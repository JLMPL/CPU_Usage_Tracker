#include "cutter.h"
#include <stdarg.h>

#define MAX_LOG_ENTRY_LENGTH 512

//unix only
#define RED     "\x1B[1;31m"
#define BLUE    "\x1B[1;34m"
#define PURPLE  "\x1B[0;35m"
#define RESET   "\x1B[1;0m"

#define LOG_FILE "log.txt"

typedef struct
{
    log_level_t level;
    char str[MAX_LOG_ENTRY_LENGTH];
}log_entry_t;

static sem_t log_entries_available_semaphore;
static pthread_mutex_t log_entries_mutex;

static log_entry_t log_entry_queue;
static int log_entry_queue_size = 0;

// called from other threads
void logger_log(log_level_t level, const char* str, ...)
{
    char buffer[MAX_LOG_ENTRY_LENGTH];

    va_list args;
    va_start(args, str);
    // -Wformat-nonliteral - not supposed to be
    vsnprintf(buffer, MAX_LOG_ENTRY_LENGTH, str, args);
    va_end(args);

    switch (level)
    {
        case LOG_INFO:
            printf(BLUE "[info] " RESET "%s", buffer);
            break;
        case LOG_WARNING:
            printf(PURPLE "[warning] " RESET "%s", buffer);
            break;
        case LOG_ERROR:
            printf(RED "[error] " RESET "%s", buffer);
            break;
    }

    log_entry_t le;
    le.level = level;
    memcpy(le.str, buffer, MAX_LOG_ENTRY_LENGTH);

    pthread_mutex_lock(&log_entries_mutex);
        log_entry_queue = le;
        log_entry_queue_size++;
    pthread_mutex_unlock(&log_entries_mutex);
    sem_post(&log_entries_available_semaphore);
}

// apparently min & max are not to be taken for granted
static int ct_min(int a, int b)
{
    if (a < b)
        return a;

    return b;
}

static void* logger_job(void* data)
{
    FILE* file = fopen(LOG_FILE, "a");

    if (!file)
    {
        printf("Logging to file unavailable!\n");
    }

    while (true)
    {
        sem_wait(&log_entries_available_semaphore);
        // pthread_mutex_lock(&log_entries_mutex);
        logger_log(log_entry_queue.level, log_entry_queue.str);
        log_entry_queue_size--;

        // int length = ct_min((int)strlen(log_entry_queue.str), MAX_LOG_ENTRY_LENGTH);
        // fwrite(log_entry_queue.str, 1, (ulong)length, file);
        // pthread_mutex_unlock(&log_entries_mutex);
    }

    //fclose is handled by exit(int)
    return NULL;
}

void logger_job_init(job_t* logger, sync_primitives_t* sync_prims)
{
    memset(logger, 0, sizeof(job_t));

    pthread_mutex_init(&log_entries_mutex, NULL);
    sem_init(&log_entries_available_semaphore, 0, 0);

    logger->args = (job_arguments_t){
        .prims = sync_prims,
        .buffers = NULL
    };

    pthread_create(&logger->thread, NULL, &logger_job, &logger->args);
}

void logger_destroy(void)
{
    pthread_mutex_destroy(&log_entries_mutex);
    sem_destroy(&log_entries_available_semaphore);
}
