#include "cutter.h"

int main(void)
{
    sig_catcher_init();

    while (true)
    {
        proc_stat_info_t info;
        reader_read_proc_stat(&info);

        computed_info_t c_info;
        analyzer_compute_usage(&info, &c_info);

        printf("cpu %d\n", c_info.cpu.value);

        for (int i = 0; i < c_info.num_cores; i++)
        {
            printf("cpu%d %d\n", i, c_info.cores[i].value);
        }

        sleep(1);
    }

    return 0;
}
