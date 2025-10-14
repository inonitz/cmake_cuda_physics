#ifndef __UTIL2_C_DEFINITION_SLEEP_OSX__
#define __UTIL2_C_DEFINITION_SLEEP_OSX__
#include <stdint.h>
#include "util2/C/util2_api.h"
#include "util2/C/util2_extern.h"


UTIL2_EXTERNC_DECL_BEGIN

UTIL2_API void     microsleep_osx(uint32_t microseconds);
UTIL2_API void     millisleep_osx(uint32_t milliseconds);
UTIL2_API uint64_t nowms_osx();

UTIL2_EXTERNC_DECL_END


#endif /* __UTIL2_C_DEFINITION_SLEEP_OSX__ */
