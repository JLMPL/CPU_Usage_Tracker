#include "cutter.h"

void printer_print_formatted(computed_info_t* c_info)
{
    system("clear");
    logger_log(LOG_INFO, "pid %d\n", getpid());
    printf("total\t%d\n\n", c_info->cpu.value);

    for (int i = 0; i < c_info->num_cores; i++)
    {
        printf("cpu%d\t%d\n", i, c_info->cores[i].value);
    }
}
