#include "cutter.h"

static noreturn void term()
{
#ifndef BUILD_TEST
    ct_shutdown();
#else
    exit(0);
#endif
}

void sig_catcher_init(void)
{
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);
}
