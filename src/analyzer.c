#include "cutter.h"

static int compute_for_core(proc_stat_core_info_t* core_info)
{
    core_info->user = core_info->user - core_info->guest;
    core_info->nice     = core_info->nice - core_info->guest_nice;

    int idlealltime   = core_info->idle + core_info->iowait;
    int systemalltime = core_info->system + core_info->irq + core_info->softirq;
    int virtalltime   = core_info->guest + core_info->guest_nice;

    int total = core_info->user + core_info->nice + systemalltime + idlealltime + core_info->steal + virtalltime;

    return ((float)(total - idlealltime) / (float)total) * 100;

    float busy_time = (float)total - (float)idlealltime;
    float busy_perc = busy_time / (float)total;

    return (int)(busy_perc * 100.f);
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
