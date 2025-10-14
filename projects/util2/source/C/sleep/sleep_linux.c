#include <util2/C/sleep/sleep_linux.h>
#include <time.h>
#include <errno.h>


void microsleep_linux(uint32_t microseconds) {
    struct timespec ts;
    ts.tv_sec = microseconds / 1000;
    ts.tv_nsec = (microseconds % 1000) * 1000000;

    while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
    return;
}


void millisleep_linux(uint32_t milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000000;
    ts.tv_nsec = (milliseconds % 1000000) * 1000;

    while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
    return;
}


uint64_t nowms_linux() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return ( (uint64_t)now.tv_sec ) * 1000000 + now.tv_nsec / 1000;
}