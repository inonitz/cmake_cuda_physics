#ifndef __UTIL2_C_DEFINITION_SLEEP_LINUX__
#define __UTIL2_C_DEFINITION_SLEEP_LINUX__
#include <stdint.h>
#include "util2/C/util2_api.h"
#include "util2/C/util2_extern.h"


UTIL2_EXTERNC_DECL_BEGIN

UTIL2_API void     microsleep_linux(uint32_t microseconds);
UTIL2_API void     millisleep_linux(uint32_t milliseconds);
UTIL2_API uint64_t nowms_linux();

UTIL2_EXTERNC_DECL_END


#endif /* __UTIL2_C_DEFINITION_SLEEP_LINUX__ */
