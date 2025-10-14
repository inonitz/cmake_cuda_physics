#include <util2/C/sleep/sleep_win32.h>
#include <stdlib.h>
#include <util2/C/macro.h>
#include <util2/C/tinycthread.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


static HANDLE   s_wintimer        = NULL;
static uint64_t s_wintimerfreq    = 0;
static double   s_wintimerinvfreq = 0;
static mtx_t    s_exitMutex;


static void closeTimerHandle() {
    CloseHandle(s_wintimer);
    return;
}

void microsleep_win32(uint32_t microseconds) {
    Sleep(microseconds);
    return;
}


void millisleep_win32(uint32_t milliseconds) {
    LARGE_INTEGER ft;
    ft.QuadPart = -1 * __scast(int64_t, milliseconds);  // '-' using relative time

    
    if(unlikely(s_wintimer == NULL)) {
        /* first time we pay the price for the branch, next time will be fine */
        mtx_init(&s_exitMutex, mtx_plain | mtx_recursive);
        s_wintimer = CreateWaitableTimer(NULL, TRUE, NULL);
        if(atexit(closeTimerHandle) != 0) {
            while(mtx_trylock(&s_exitMutex) != thrd_success) {}
            exit(-1);
            mtx_unlock(&s_exitMutex);
        }
    }


    SetWaitableTimer(s_wintimer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(s_wintimer, INFINITE);
    return;
}


uint64_t nowms_win32() {
    LARGE_INTEGER tmp;
    double        accurateResult = 0;

    if( unlikely(s_wintimerinvfreq == 0) ) {
        /* first time we pay the price for the branch, next time will be fine */
        QueryPerformanceFrequency(&tmp);
        s_wintimerfreq = tmp.QuadPart;
        s_wintimerinvfreq = 1.0 / s_wintimerinvfreq;
    }


    QueryPerformanceCounter(&tmp);
    accurateResult = tmp.QuadPart * 1000000;
    accurateResult *= s_wintimerinvfreq;
    return __scast(uint64_t, accurateResult);
}

