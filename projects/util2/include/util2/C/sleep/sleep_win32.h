#ifndef __UTIL2_C_DEFINITION_SLEEP_WIN32__
#define __UTIL2_C_DEFINITION_SLEEP_WIN32__
#include <stdint.h>
#include "util2/C/util2_api.h"
#include "util2/C/util2_extern.h"


UTIL2_EXTERNC_DECL_BEGIN

UTIL2_API void     microsleep_win32(uint32_t microseconds);
UTIL2_API void     millisleep_win32(uint32_t milliseconds);
UTIL2_API uint64_t nowms_win32();

UTIL2_EXTERNC_DECL_END


#endif /* __UTIL2_C_DEFINITION_SLEEP_WIN32__ */
