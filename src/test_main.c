#include "cutter.h"

#define TEST(x) static bool test__##x(void)
#define EXECUTE_TEST(x) if (test__##x()) { printf("%s passed\n", #x); } else { printf("%s failed\n", #x); }

TEST(reader_read_proc_stat)
{
    proc_stat_info_t ps_info;
    if (reader_read_proc_stat(&ps_info, "proc_stat") == CT_FAILURE)
        return false;

    if (ps_info.cpu.user != 299563)
        return false;

    if (ps_info.cores[0].user != 27025)
        return false;

    if (ps_info.cores[2].nice != 16562)
        return false;

    return true;
}

int main()
{
    EXECUTE_TEST(reader_read_proc_stat)

    return 0;
}
