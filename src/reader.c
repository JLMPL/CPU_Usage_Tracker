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
