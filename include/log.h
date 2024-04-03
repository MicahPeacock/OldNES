#ifndef OLDNES_LOG_H
#define OLDNES_LOG_H

#include <stdio.h>
#include <stdint.h>

#define PRINTF(...) printf(__VA_ARGS__)

enum LogLevel {
    DEBUG = 0,
    ERROR,
    INFO,

};

void LOG(enum LogLevel level, const char* fmt, ...);

#endif //OLDNES_LOG_H
