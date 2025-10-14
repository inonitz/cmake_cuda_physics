#ifndef __SDL3_CORRECTNESS_CHECK_DEFINITION_HEADER__
#define __SDL3_CORRECTNESS_CHECK_DEFINITION_HEADER__
#include <util2/C/macro.h>


#ifndef SDLCHECK
#	define SDLCHECK(condition, msg)                             \
		if ( unlikely( !!(condition) ) ) {                      \
			SDL_Log("SDL_ERROR [%s]: %s", SDL_GetError(), msg); \
			return SDL_APP_FAILURE;                             \
		}
#else
#	define SDLCHECK(condition, format, ...) \
		do {                                 \
		} while (0);
#endif


#ifndef SDLSTATUS
#	define SDLSTATUS(condition, msg) \
		if ( unlikely( !!(condition) ) ) {                      \
			SDL_Log("SDL_ERROR [%s]: %s", SDL_GetError(), msg); \
		}
#else
#	define SDLSTATUS(condition, msg) do {} while(0);
#endif


#ifndef SDLSTATUSFMT
#	define SDLSTATUSFMT(condition, msg, ...) \
		if ( unlikely( !!(condition) ) ) {                      \
			SDL_Log("[SDL_ERROR] -> '%s'\n[SDL_ERROR] -> \b", SDL_GetError(), msg); \
			SDL_Log(msg, __VA_ARGS__); \
		}
#else
#	define SDLSTATUSFMT(condition, msg) do {} while(0);
#endif



#endif /* __SDL3_CORRECTNESS_CHECK_DEFINITION_HEADER__ */
