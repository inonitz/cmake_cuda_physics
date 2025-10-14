#ifndef __UTIL2_C_DEFINITION_SLEEP__
#define __UTIL2_C_DEFINITION_SLEEP__
#include <stdint.h>
#include "util2_api.h"
#include "util2/C/util2_extern.h"


UTIL2_EXTERNC_DECL_BEGIN

UTIL2_API void microsleep(uint32_t microseconds);
UTIL2_API void millisleep(uint32_t milliseconds);
UTIL2_API uint64_t nowms();

UTIL2_EXTERNC_DECL_END


#endif /* __UTIL2_C_DEFINITION_SLEEP__ */
