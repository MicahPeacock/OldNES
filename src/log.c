#include <stdarg.h>

#include "log.h"

void LOG(enum LogLevel level, const char* fmt, ...) {
//    if (level < LOG_LEVEL) return;
    switch (level) {
        case ERROR:
            printf("[ERROR] ");
            break;
        case INFO:
            printf("[INFO]  ");
            break;
        case DEBUG:
            printf("[DEBUG] ");
            break;
        default:
            printf("[LOG]   ");
    }
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}