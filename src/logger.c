#include "cutter.h"
#include <stdarg.h>

#define MAX_LOG_ENTRY_LENGTH 512
#define MAX_LOG_ENTRIES 512

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

typedef struct
{
    log_entry_t entries[MAX_LOG_ENTRIES];
    int num_entries;
    int read_index;
}log_queue_t;

static sem_t log_entries_available_semaphore;
static pthread_mutex_t log_entries_mutex;

static atomic_bool queue_flag = false;
static log_queue_t queues[2];

static void enqueue_on_write_queue(log_level_t level, const char* str)
{
    log_entry_t entry;
    entry.level = level;
    memcpy(entry.str, str, MAX_LOG_ENTRY_LENGTH);

    int write_index = (int)atomic_load(&queue_flag);

    if (queues[write_index].num_entries >= MAX_LOG_ENTRIES)
        return;

    queues[write_index].entries[queues[write_index].num_entries++] = entry;
}

static bool pop_from_read_queue(log_entry_t* entry)
{
    bool raw_bool = !(bool)atomic_load(&queue_flag);
    int read_index = (int)raw_bool;

    if (queues[read_index].num_entries <= 0)
    {
        return false;
    }

    if (queues[read_index].read_index == queues[read_index].num_entries)
    {
        return false;
    }

    *entry = queues[read_index].entries[queues[read_index].read_index];
    queues[read_index].read_index++;

    return true;
}

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

    enqueue_on_write_queue(level, buffer);
}

// apparently min & max are not to be taken for granted
static int ct_min(int a, int b)
{
    if (a < b)
        return a;

    return b;
}

static void* logger_job()
{
    FILE* file = fopen(LOG_FILE, "a");

    memset(queues, 0, sizeof(log_queue_t) * 2);

    if (!file)
    {
        printf("Logging to file unavailable!\n");
    }

    while (true)
    {
        log_entry_t entry;

        if (pop_from_read_queue(&entry))
        {
            int length = ct_min((int)strlen(entry.str), MAX_LOG_ENTRY_LENGTH);
            fwrite(entry.str, 1, (ulong)length, file);
            fflush(file);
        }
        else
        {
            atomic_store(&queue_flag, !atomic_load(&queue_flag));
        }
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
