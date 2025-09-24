#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <new>
#include <cmath>
#include <SDL3/SDL_main.h>
#include <util2/C/macro.h>
#include "check.hpp"
#include "scenetest/context.hpp"


#ifdef _DEBUG
#	define APP_DEBUG_MODE 1
#else
#	define APP_DEBUG_MODE 0
#endif


static AppContext*   g_appctx       = nullptr;
static constexpr u16 k_windowWidth  = 1920;
static constexpr u16 k_windowHeight = 1080;
static constexpr f64 k_a0           = 1;
static constexpr u32 k_quantumNumN  = 5;
static constexpr u32 k_quantumNumL  = 3;
static constexpr i32 k_quantumNumM  = -1;
static constexpr f64 k_radius       = 100;
static constexpr u64 k_sampleCountX = 1000;
static constexpr u64 k_sampleCountY = 1000;
static constexpr u64 k_sampleCountZ = 1000;
static constexpr f64 k_pi           = 3.14159265358979311599796346854;


auto SDL_AppInit(void** appstate, int argc, char* argv[]) -> SDL_AppResult {
	SDL_AppResult    status{SDL_APP_CONTINUE};

	status = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == true ? SDL_APP_CONTINUE : SDL_APP_FAILURE;
	SDLCHECK(status, "Failure to initialize Video &/ Event Subsystem\n");


	g_appctx = new (SDL_malloc(sizeof(AppContext))) AppContext;
	SDLCHECK(g_appctx == nullptr, "Failure to allocate global context\n");


	status = g_appctx->create(APP_DEBUG_MODE, k_windowWidth, k_windowHeight);
	SDLCHECK(status, "Failure to Initialize global app Context\n");


	return SDL_APP_CONTINUE;
}


auto SDL_AppEvent(void* appstate, SDL_Event* event) -> SDL_AppResult {
	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	}
	if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
		return SDL_APP_SUCCESS;
	}


	return SDL_APP_CONTINUE;
}


auto SDL_AppIterate(void* appstate) -> SDL_AppResult {
	SDLCHECK(g_appctx->update(), "Failure in App Update");

#if defined(MEASURE_PERFORMANCE_TIMEOUT_FLAG)
#	if defined(MEASURE_PERFORMANCE_TIMEOUT_ITERATIONS)
#		if (MEASURE_PERFORMANCE_TIMEOUT_FLAG == 1)
			if (g_appctx->getFrameCount() > MEASURE_PERFORMANCE_TIMEOUT_ITERATIONS) {
				return SDL_APP_SUCCESS;
			}
#		endif
#	endif
#endif
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	g_appctx->destroy();
	return;
}
