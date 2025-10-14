#include "util2/C/sleep.h"
#ifdef __APPLE__
#   include <util2/C/sleep/sleep_osx.h>
#elif defined(__linux__)
#   include <util2/C/sleep/sleep_linux.h>
#elif defined(_WIN32)
#   include <util2/C/sleep/sleep_win32.h>
#else
#   include <util2/C/compiler_warning.h>
#   pragma message WARN("Couldn't detect a supported platform - Implementation for sleep.h doesn't exist.")
#endif /* __APPLE__ */




UTIL2_API void microsleep(uint32_t microseconds)
{
#ifdef __APPLE__
    microsleep_osx(microseconds);
#elif defined(__linux__)
    microsleep_linux(microseconds);
#elif defined(_WIN32)
    microsleep_win32(microseconds);
#else

#endif /* __APPLE__ */
    return;
}


UTIL2_API void millisleep(uint32_t milliseconds)
{
#ifdef __APPLE__
    millisleep_osx(milliseconds);
#elif defined(__linux__)
    millisleep_linux(milliseconds);
#elif defined(_WIN32)
    millisleep_win32(milliseconds);
#else
    
#endif /* __APPLE__ */
    return;
}


UTIL2_API uint64_t nowms()
{
#ifdef __APPLE__
    return nowms_osx();
#elif defined(__linux__)
    return nowms_linux();
#elif defined(_WIN32)
    return nowms_win32();
#else
    return -1;
#endif /* __APPLE__ */
}

