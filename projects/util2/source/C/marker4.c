#include "util2/C/marker4.h"


// #if UTIL2_MARKER_FLAG_DEFINE_IMPLEMENTATION == 1
// #   if defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 199309L)
// #       include <time.h>
// #   else
// #       include "util2/C/compiler_warning.h"
// #       pragma message WARN("Cannot use <time.h> - No support available for it")
// #   endif
// #   include "util2/C/macro.h"
// #   include <stdatomic.h>
// #   include <stdio.h>
// #   include <stdlib.h>
// #   include <stdarg.h>
// #   include <pthread.h>
// #if defined __clang__ && defined _WIN32 && defined __MINGW64__
// #   include <pthread_time.h>
// #endif


#if UTIL2_MARKER_FLAG_DEFINE_IMPLEMENTATION == 1
#   include "util2/C/macro.h"
#   include "util2/C/print.h"
#   include <stdatomic.h>
#   include <stdio.h>
#   include <stdlib.h>
#   include <stdarg.h>


static alignsz(64) atomic_uint_least64_t s_markflag;
static FILE*                             s_logbuffer;


static void util2_marker_flag_destroy_state()
{
#if UTIL2_MARKER_FLAG_LOG_TO_FILE == 1
    fclose(s_logbuffer);
#endif
    return;
}

static void util2_marker_flag_create_state_once()
{
    if(likely( atomic_load(&s_markflag) != 0 )) 
        return;

    s_logbuffer = (UTIL2_MARKER_FLAG_LOG_TO_FILE == 1) ? fopen("__marker_log.txt", "w") : stdout;
    atexit(&util2_marker_flag_destroy_state);
    return;
}


// void util2_marker_flag(
//     const char*   file_macro, 
//     unsigned long line_macro, 
//     const char*   formatstr, 
//     ...
// ) {
//     __begin_exclusion();
//     if(unlikely( atomic_load(&s_markflag) == 0 )) 
//         __util2_marker_flag_create_state();

//     __util2_internal_printfmt("[%llu] %s:%u", atomic_load(&s_markflag), file_macro, line_macro);
//     __util2_internal_printstr(" [ADDITIONAL_INFO] ");
//     va_list fmt_list;
//     va_start(fmt_list, formatstr);
//     __util2_internal_printfmt_va_list(formatstr, fmt_list);
//     va_end(fmt_list);


// #if UTIL2_MARKER_FLAG_INCLUDE_NEWLINE_AT_END == 1
//     __util2_internal_printstr("\n");
// #endif
//     atomic_fetch_add_explicit(&s_markflag, 1, memory_order_relaxed);
//     __end_exclusion();
//     return;
// }



void util2_marker_flag(
    const char*   file_macro, 
    unsigned long line_macro, 
    const char*   formatstr, 
    ...
) {
    util2_marker_flag_create_state_once();


    util2_fprintf(s_logbuffer, "[%6llu] %s:%-4u [ADDITIONAL_INFO] ", atomic_load(&s_markflag), file_macro, line_macro);
    va_list fmt_list;
    va_start(fmt_list, formatstr);
    util2_va_fprintf(s_logbuffer, formatstr, fmt_list);
    va_end(fmt_list);


#if UTIL2_MARKER_FLAG_INCLUDE_NEWLINE_AT_END == 1
    util2_fputs(s_logbuffer, "\n");
#endif
    atomic_fetch_add_explicit(&s_markflag, 1, memory_order_relaxed);
    return;
}


#endif /* UTIL2_MARKER_FLAG_DEFINE_IMPLEMENTATION */