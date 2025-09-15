#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <util2/C/base_type.h>
#include <array>
#include <cmath>
#include <hellocuda.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


static constexpr u16 windowWidth  = 1080;
static constexpr u16 windowHeight = 720;
static constexpr u16 pointCount   = 500;
static constexpr u16 pixelsPerSecMin = 30;
static constexpr u16 pixelsPerSecMax = 60;
static SDL_Window*   window    = nullptr;
static SDL_Renderer* renderer  = nullptr;
static u64        	 last_time = 0;
static std::array<SDL_FPoint, pointCount> points;
static std::array<f32, pointCount> 		  point_speeds;


auto SDL_AppInit(void** appstate, int argc, char* argv[]) -> SDL_AppResult {
	SDL_SetAppMetadata("3D Scene with camera class", "1.0", "");


	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	if (!SDL_CreateWindowAndRenderer("", windowWidth, windowHeight, 0, &window, &renderer)) {
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	/* set up the data for a bunch of points. */
	for (uint32_t i = 0; i < SDL_arraysize(points); i++) {
		points[i].x     = SDL_randf() * ((float) windowWidth);
		points[i].y     = SDL_randf() * ((float) windowHeight);
		point_speeds[i] = pixelsPerSecMin + (SDL_randf() * (pixelsPerSecMax - pixelsPerSecMin));
	}

	last_time = SDL_GetTicks();


	return SDL_APP_CONTINUE;
}


auto SDL_AppEvent(void* appstate, SDL_Event* event) -> SDL_AppResult {
	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS; 
	}
	return SDL_APP_CONTINUE;
}


auto SDL_AppIterate(void* appstate) -> SDL_AppResult {
	const Uint64 now     = SDL_GetTicks();
	const float  elapsed = ((float) (now - last_time)) / 1000.0f;
	int          i;


	for (i = 0; i < points.size(); i++) {
		const float distance = elapsed * point_speeds[i];
		points[i].x += distance;
		points[i].y += distance;
		if ((points[i].x >= windowWidth) || (points[i].y >= windowHeight)) {
			/* off the screen; restart it elsewhere! */
			if (SDL_rand(2)) {
				points[i].x = SDL_randf() * ((float) windowWidth);
				points[i].y = 0.0f;
			} else {
				points[i].x = 0.0f;
				points[i].y = SDL_randf() * ((float) windowHeight);
			}
			point_speeds[i] = pixelsPerSecMin + (SDL_randf() * (pixelsPerSecMax - pixelsPerSecMin));
		}
	}

	last_time = now;

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderPoints(renderer, points.data(), points.size()); 
	SDL_RenderPresent(renderer);
	return SDL_APP_CONTINUE;
}


void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	return;
}