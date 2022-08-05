#include "cutter.h"

static noreturn void term()
{
    ct_shutdown();
}

void sig_catcher_init(void)
{
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);
}
