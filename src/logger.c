#include "cutter.h"
#include <stdarg.h>

#define MAX_LOG_ENTRY_LENGTH 512

//unix only
#define RED     "\x1B[1;31m"
#define BLUE    "\x1B[1;34m"
#define PURPLE  "\x1B[0;35m"
#define RESET   "\x1B[1;0m"

void logger_log(logLevel_t level, const char* str, ...)
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
}
