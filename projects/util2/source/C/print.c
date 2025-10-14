#include "util2/C/print.h"
#include "util2/C/macro.h"
#include "util2/C/tinycthread.h"
#include "util2/C/sleep.h" 
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


#ifdef UTIL2_PRINT_LOG_TO_FILE
#   undef UTIL2_PRINT_LOG_TO_FILE
#   define UTIL2_PRINT_LOG_TO_FILE 1
#else
#   define UTIL2_PRINT_LOG_TO_FILE 0
#endif


#define __UTIL2_GENERIC_FORMAT_BUFFER_MAX_SIZE 32768
typedef struct __generic_format_char_buffer 
{
    char mem[__UTIL2_GENERIC_FORMAT_BUFFER_MAX_SIZE];
} util2_format_buffer;


static alignsz(64) mtx_t                 s_write_lock;
static alignsz(64) atomic_uint_least64_t s_onceflag;
static FILE*                             s_defaultlogbuf = NULL;
static FILE*                             s_currlogbuffer = NULL;


static void __util2_begin_exclusion()
{
    while( mtx_trylock(&s_write_lock) != thrd_success)
        millisleep(5);

    return;
}

static void __util2_end_exclusion()
{
    mtx_unlock(&s_write_lock);
    return;
}


static void util2_destroy_state()
{
    /* Repeatedly try to close the handle until success. This will be fun to debug */
#if UTIL2_PRINT_LOG_TO_FILE == 1
    while(fclose(s_defaultlogbuf) == EOF) {
        millisleep(5);
    }
#endif
    mtx_destroy(&s_write_lock);
    return;
}

static void util2_init_state_once()
{
    if(unlikely( atomic_load(&s_onceflag) != 0 ))
        return;

    mtx_init(&s_write_lock, mtx_plain | mtx_recursive);

    __util2_begin_exclusion();
    s_defaultlogbuf = UTIL2_PRINT_LOG_TO_FILE == 1 ? fopen("printlog.txt", "w") : NULL;
    s_currlogbuffer = UTIL2_PRINT_LOG_TO_FILE == 1 ? s_defaultlogbuf : stdout;
    if(atexit(util2_destroy_state) != 0) {
        exit(-1);
    }
    __util2_end_exclusion();

    /* on next call the function will return */
    atomic_fetch_add_explicit(&s_onceflag, 1, memory_order_relaxed);
    return;
}




void util2_va_fprintf(
    FILE*       write_into,
    const char* format, 
    va_list     arg_list
) {
    static util2_format_buffer fmtbuf;
    va_list arglistcopy;
    int size = 0;
    int done = 1;
    unsigned char invalid_state = BOOL_FALSE;


    fmtbuf.mem[__UTIL2_GENERIC_FORMAT_BUFFER_MAX_SIZE - 1] = '\0'; /* incase of overflow */

    /* use arglistcopy to calculate the total bytes needed */
    va_copy(arglistcopy, arg_list);
    size = 1 + vsnprintf(NULL, 0, format, arglistcopy);
    va_end(arglistcopy);

    if (size > __UTIL2_GENERIC_FORMAT_BUFFER_MAX_SIZE) {
        util2_fputs("\n[print.c] => util2_va_fprintf() __VA_ARGS__ too large\n", write_into);
        invalid_state = BOOL_TRUE;
    }
    if (!invalid_state) { /* on success iterate-over/use the given arg_list inside vsnprintf, va_end() will be called outside the function */
        done = vsnprintf(fmtbuf.mem, size, format, arg_list);
    }


    if (invalid_state || done < 0) {
        util2_fputs("\n[print.c] => util2_va_fprintf() Couldn't format __VA_ARGS__\n", write_into);
        fmtbuf.mem[0] = '\0'; /* if fputs encounters an eol it'll stop */
    }
    util2_fputs(fmtbuf.mem, write_into);
    return;
}


void util2_fprintf(FILE* write_to, const char* format, ...)
{
    util2_init_state_once();

    __util2_begin_exclusion();
    va_list arg_list;
    va_start(arg_list, format);
    util2_va_fprintf(write_to, format, arg_list);
    va_end(arg_list);
    __util2_end_exclusion();
    return;
}


void util2_printf(const char* format, ...)
{
    util2_init_state_once();

    __util2_begin_exclusion();
    va_list arg_list;
    va_start(arg_list, format);
    util2_va_fprintf(s_currlogbuffer, format, arg_list);
    va_end(arg_list);
    __util2_end_exclusion();
    return;
}


void util2_fputs(const char* str, FILE* write_to) {
    /* thread safe as long as handle is valid */
    util2_init_state_once();
    
    __util2_begin_exclusion();
    int status = fputs(str, write_to);
    if(unlikely( status == EOF )) 
    {
        status = fputs("\n[print.c] => util2_fputs() Couldn't Write to File Handle 'write_to'\n", stderr);

        if(status == EOF) {
            exit(-1);
        }
    }
    __util2_end_exclusion();
    return;
}


void util2_puts(const char* str) {
    util2_fputs(str, s_currlogbuffer);
    return;
}


void util2_setfs(FILE* newfilestream) {
    s_currlogbuffer = (newfilestream == NULL) ? s_defaultlogbuf : newfilestream;
    return;
}

