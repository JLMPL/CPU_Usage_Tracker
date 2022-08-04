#include "cutter.h"

int main(void)
{
    sig_catcher_init();

    proc_stat_info_t info;
    reader_read_proc_stat(&info);

    printf("I'm running!\n");
    return 0;
}
