#define _POSIX_C_SOURCE 200809L
#include "util.h"
#include <time.h>
#include <stdio.h>

uint64_t tp_now_epoch_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)(ts.tv_nsec / 1000000ULL);
}

void tp_iso_timestamp(char *buf, size_t n) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm tm;
    localtime_r(&ts.tv_sec, &tm);
    snprintf(buf, n, "%04d-%02d-%02dT%02d:%02d:%02d.%03ld",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                tm.tm_hour, tm.tm_min, tm.tm_sec, ts.tv_nsec / 1000000);
}

double tp_bytes_to_mib(uint64_t b) {
    return (double)b / (1024.0 * 1024.0);
}
