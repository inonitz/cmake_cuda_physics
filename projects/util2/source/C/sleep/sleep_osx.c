#include <util2/C/sleep/sleep_osx.h>
#include <time.h>
#include <errno.h>
#include <mach/clock.h>
#include <mach/mach.h>


void microsleep_osx(uint32_t microseconds) {
    struct timespec ts;
    ts.tv_sec = microseconds / 1000;
    ts.tv_nsec = (microseconds % 1000) * 1000000;

    while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
    return;
}


void millisleep_osx(uint32_t milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000000;
    ts.tv_nsec = (milliseconds % 1000000) * 1000;

    while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
    return;
}


uint64_t nowms_osx() {
    clock_serv_t cs;
    mach_timespec_t ts;

    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cs);
    clock_get_time(cs, &ts);
    mach_port_deallocate(mach_task_self(), cs);

    return ( ((uint64_t)ts.tv_sec) * 1000000 ) + (ts.tv_nsec / 1000);
}