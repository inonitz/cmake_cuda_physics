#include <algorithm>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <colourmap_buf.hpp>
#include <util2/C/marker4.h>
#include <util2/C/print.h>
#include <util2/vec2.hpp>
#include <util2/time.hpp>
#include <vector>
#include <cmath>
#include <limits>
#include <hello.hpp>
#include <hellocuda.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


#ifndef SDLCHECK
#	define SDLCHECK(condition, format, ...) \
		if(!!(condition)) { \
			SDL_Log("SDL_ERROR [%s]:", SDL_GetError()); \
			SDL_Log(format, __VA_ARGS__); \
			return SDL_APP_FAILURE; \
		}
#else
#	define SDLCHECK(condition, format, ...) do {} while(0);
#endif


struct colour_rgba {
	u8 rgba[4];
};


static SDL_Window*    g_window          = nullptr;
static SDL_Renderer*  g_renderer        = nullptr;
static SDL_GPUDevice* g_gpu = nullptr;
static u64           g_lasttime        = 0;
static u64           g_currtime        = 0;
static u64           g_timeDelta       = 0;
static constexpr u16 k_windowWidth     = 1920;
static constexpr u16 k_windowHeight    = 1080;
static constexpr u16 k_pixelsPerSecMin = 30;
static constexpr u16 k_pixelsPerSecMax = 60;
static constexpr f64 k_a0              = 1;
static constexpr u32 k_quantumNumN 	   = 2;
static constexpr u32 k_quantumNumL 	   = 1;
static constexpr i32 k_quantumNumM 	   = 0;
static constexpr f64 k_radius          = 10;
static constexpr u64 k_sampleCountX    = 1000;
static constexpr u64 k_sampleCountY    = 1000;

static std::vector<f64> xrange;
static std::vector<f64> zrange;
static std::vector<f64> xmeshgrid;
static std::vector<f64> zmeshgrid;
static std::vector<f64> radiusPlane;
static std::vector<f64> thetaPlane;
static std::vector<f64> psi;


static std::vector<SDL_FPoint>  g_pointbuf;
static std::vector<colour_rgba> g_colourbuf;


constexpr auto pi() -> double { return std::acos(-1); }
constexpr auto spherical_harmonic_legendre(f64 theta) -> f64
{
	/* taken directly from the eq for P_(-m, l): https://en.wikipedia.org/wiki/Spherical_harmonics#Orthogonality_and_normalization */
	constexpr u32 k_Mfactor = (k_quantumNumM < 0) ? __scast(u32, -k_quantumNumM) : __scast(u32, k_quantumNumM);

	f64 result = std::assoc_legendre(k_quantumNumL, k_Mfactor, std::cos(theta));
	if constexpr (k_quantumNumM < 0) {
		f64 tmp = std::pow(-1, k_Mfactor);
		tmp *= std::tgamma(k_quantumNumL - k_Mfactor + 1);
		tmp /= std::tgamma(k_quantumNumL + k_Mfactor + 1); 
		result *= tmp;
	}
	return result;
}


/*
	for the X and Z coordiate spaces
*/
void generateLinearSpace(
	f64 extentBegin, 
	f64 extentEnd, 
	u64 samples, 
	std::vector<f64>& toInit
) {
	f64 extentDelta = __scast(f64, samples);
	extentDelta = (extentEnd - extentBegin) / extentDelta;


	toInit.resize(samples);
	for(auto i = 0; i < samples; ++i) {
		toInit[i] = extentBegin + extentDelta * i;
	}
	return;
}


void generateMeshgrid(
	std::vector<f64> const& rangeX,
	std::vector<f64> const& rangeY,
	std::vector<f64>& 		meshGridX,
	std::vector<f64>& 		meshGridY
) {
	const auto matrixGridSize = rangeX.size() * rangeY.size();
	meshGridX.resize(matrixGridSize);
	meshGridY.resize(matrixGridSize);
	for(u32 j = 0; j < rangeY.size(); ++j) {
		for(u32 i = 0; i < rangeX.size(); ++i) {
			meshGridY[i + j * rangeX.size()] = rangeY[j];
			meshGridX[i + j * rangeX.size()] = rangeX[i];
		}
	}


	return;
}


void generateRadiiAndSphericalCoordinatesBuffer(
	std::vector<f64> const& planeX, 
	std::vector<f64> const& planeZ,
	std::vector<f64>& 		planeRadii,
	std::vector<f64>& 		planePolar
) {
	ifcrash(planeX.size() != planeZ.size());

	planeRadii.resize(planeX.size());
	planePolar.resize(planeX.size());
	for(auto i = 0; i < planeX.size(); ++i) {
		planeRadii[i] = std::sqrt(planeX[i]*planeX[i] + planeZ[i]*planeZ[i]);
		planePolar[i] = std::atan2(planeX[i], planeZ[i]);
	}
	return;
}


auto calculateNormalizationFactor() -> f64
{
	f64 tmp0 = std::pow(2 / (k_quantumNumN * k_a0), 3);
	f64 tmp1 = 0;
	f64 tmp2 = 0;

	tmp1 = std::tgamma(k_quantumNumN - k_quantumNumL);
	tmp2 = 2 * k_quantumNumN * std::pow(
		std::tgamma(k_quantumNumN + k_quantumNumL + 1), 
		3
	);
	tmp2 = 1.0 / tmp2;
	
	return std::sqrt(tmp0 * tmp1 * tmp2); 
}


auto calculateRadialPart(f64 radius) -> f64
{
	constexpr f64 div = 1 / ( k_quantumNumN * k_a0);
	f64 arg, tmp;
	arg = (2 * radius * div);

	tmp = std::exp(-0.5 * arg); 
	tmp *= std::pow(arg, k_quantumNumL);
	tmp *= std::assoc_laguerre(
		k_quantumNumN - k_quantumNumL - 1,
		2 * k_quantumNumL + 1, 
		arg
	);

	return tmp;
}


void calculateAngularPartSquared(
	std::vector<f64> const& planePolar,
	std::vector<f64>& 		angularPartSq
) {
	f64 tmp = 0;
	f64 normsq = (2 * k_quantumNumL + 1) / (4 * pi());
	normsq *= std::tgamma(k_quantumNumL - k_quantumNumM + 1);
	normsq /= std::tgamma(k_quantumNumL + k_quantumNumM + 1);

	angularPartSq.resize(planePolar.size());
	angularPartSq.assign(planePolar.size(), normsq);
	for(uint32_t i = 0; i < planePolar.size(); ++i) 
	{
		/* 
			tmp was always 0 before adding *= 
			check why
		*/
		tmp = spherical_harmonic_legendre(planePolar[i]);
		angularPartSq[i] *= tmp * tmp;
	}


	return;
}


void calculateProbabilityDensity(
	std::vector<f64> const& planeRadii,
	std::vector<f64> const& planePolar,
	std::vector<f64>& probDensity
) {
	ifcrash(planePolar.size() != planeRadii.size());
	f64 tmp = 0;
	f64 N = calculateNormalizationFactor(); /* Wrong Value */


	calculateAngularPartSquared(planePolar, probDensity);
	for(u32 i = 0; i < planeRadii.size(); ++i)
	{
		tmp = N * calculateRadialPart(planeRadii[i]);
		tmp *= tmp;
		probDensity[i] *= tmp; /* probdensity[i] = angular**2 * (Normalization * Radial)**2 */
	}


	return;
}


void findRangeAndNormalize(
	std::vector<f64>& vec,
	std::array<f64, 2>& minMax
) {
	f64 min = vec[0];
	f64 max = min;

	for(u32 i = 1; i < vec.size(); ++i)  {
		min = std::min(min, vec[i]);
		max = std::max(max, vec[i]);
	}
	minMax[0] = min;
	minMax[1] = max;


	max = 1 / (max - min); /* renorm elements after getting our minmax */
	for(auto& elem : vec) {
		elem = (elem - min) * max;
	}


	return;
}


void findRange(
	std::vector<f64> const& vec,
	std::array<f64, 2>& minMax
) {
	f64 min = vec[0];
	f64 max = min;

	for(u32 i = 1; i < vec.size(); ++i)  {
		min = std::min(min, vec[i]);
		max = std::max(max, vec[i]);
	}
	minMax[0] = min;
	minMax[1] = max;
	return;
}




auto mapValueRangeToColourmap(
	f64 valMin,
	f64 valMax,
	f64 val,
	f64 const* colmap_val,
	f64 const* colmap_col,
	u64  colourmap_len
) -> colour_rgba 
{
	colour_rgba result{0, 0, 0, 1};
	f64  tmp 		= 0;
	bool skipLoop   = false;
	u32  colidx     = 0;


	val = std::clamp(val, valMin, valMax);
	if(val == valMax) {
		skipLoop = true;
		colidx = colourmap_len - 1;
	} else if(val == valMin) {
		skipLoop = true;
		colidx = 1;
	}


	if(!skipLoop) {
		val = (val - valMin) / (valMax - valMin);
		while(val > colmap_val[colidx]) {
			++colidx;
		}
	}

	/* now that we found the range for 'val' , we can interpolate between the colour values */
	valMin = colmap_val[colidx - 1];
	valMax = colmap_val[colidx];
	val = (val - valMin) / (valMax - valMin);
	for(u8 i = 0; i < 3; ++i) 
	{
		tmp = (1 - val) * colmap_col[i + (colidx - 1) * 3] + (val) * colmap_col[i + colidx * 3];
		tmp *= 255;
		result.rgba[i] = __scast(u8, tmp);
	}
	return result;
}



void writeColouredPointCommands(
	std::vector<colour_rgba> const& colours, 
	std::vector<SDL_FPoint>  const& points
) {
	ifcrash(colours.size() != points.size());


	auto* rgbptr = &colours[0].rgba[0] ;
	for(uint32_t i = 0; i < points.size(); ++i) {
		rgbptr = &colours[i].rgba[0];
		SDL_SetRenderDrawColor(g_renderer, 
			rgbptr[0],
			rgbptr[1],
			rgbptr[2],
			rgbptr[3]
		);
		SDL_RenderPoint(g_renderer, points[i].x, points[i].y);
	}
}


auto SDL_AppInit(void** appstate, int argc, char* argv[]) -> SDL_AppResult {
	constexpr i32 k_maximumRandomStates = 0x7fffffff;
	u64 randState = 0;
	std::array<f64, 2> probDensityMinMax{};
	SDL_SetAppMetadata("RenderTextureToViewport", "1.0", "");

	
	SDLCHECK(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS), "Couldn't initialize SDL\n", 
		""
	);
	SDLCHECK(!SDL_CreateWindowAndRenderer("", k_windowWidth, k_windowHeight, 0, &g_window, &g_renderer), 
		"Couldn't create window/renderer\n", ""
	);
	/* set up the data for a bunch of points. */
	g_pointbuf.resize(k_sampleCountX * k_sampleCountY);
	g_colourbuf.resize(k_sampleCountX * k_sampleCountY);

	for(u32 j = 0; j < k_sampleCountY; ++j) {
		for(u32 i = 0; i < k_sampleCountX; ++i) {
			g_pointbuf[j * k_sampleCountY + i] = SDL_FPoint{
				__scast(f32, i),
				__scast(f32, j)
			};

			SDL_rand_r(&randState, k_maximumRandomStates);
			g_colourbuf[j * k_sampleCountY + i] = colour_rgba{
				__scast(u8, (( randState >> 0 ) & 0xFF) ),
				__scast(u8, (( randState >> 8 ) & 0xFF) ),
				__scast(u8, (( randState >> 16) & 0xFF) ),
				__scast(u8, (( randState >> 24) & 0xFF) )
			};
		}
	}


	/* Prepare the data */	
	mark();
	generateLinearSpace(-k_radius, k_radius, k_sampleCountX, xrange);
	generateLinearSpace(-k_radius, k_radius, k_sampleCountY, zrange);
	generateMeshgrid(xrange, zrange, 
		xmeshgrid, 
		zmeshgrid
	);	
	mark();
	generateRadiiAndSphericalCoordinatesBuffer(
		xmeshgrid,
		zmeshgrid,
		radiusPlane,
		thetaPlane
	);
	mark();
	/* actually calculate psi**2 */
	/* PROBLEM HERE */
	calculateProbabilityDensity(
		radiusPlane,
		thetaPlane,
		psi
	);
	findRange(psi, probDensityMinMax);
	mark();

	// util2_printf("\n\npsi value range [%f, %u]\n\n", probDensityMinMax[0], probDensityMinMax[1]);
	// util2_printf("\n\npsi[i] -> Colourbuf[i]\n\n");
	g_colourbuf.resize(psi.size());
	for(u32 i = 0; i < psi.size(); ++i) {
		g_colourbuf[i] = mapValueRangeToColourmap(
			probDensityMinMax[0],
			probDensityMinMax[1],
			psi[i],
			k_colourmap_inferno_value_ranges.data(),
			k_colourmap_inferno_value_colours.data(),
			k_colourmap_inferno_buf_length
		);
		// util2_printf("%12.6f [ %u, %u, %u, %u ], ",
		// 	psi[i],
		// 	g_colourbuf[i].rgba[0], 
		// 	g_colourbuf[i].rgba[1], 
		// 	g_colourbuf[i].rgba[2], 
		// 	g_colourbuf[i].rgba[3]
		// );
		// if(i % 1000)
		// 	util2_printf("\n");

	}
	mark();

	g_lasttime = SDL_GetTicks();


	return SDL_APP_CONTINUE;
}

auto SDL_AppEvent(void* appstate, SDL_Event* event) -> SDL_AppResult {
	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	}
	return SDL_APP_CONTINUE;
}

auto SDL_AppIterate(void* appstate) -> SDL_AppResult {
	g_currtime = SDL_GetTicks();
	g_timeDelta = (g_currtime - g_lasttime);
	g_lasttime = g_currtime;


	SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(g_renderer);
	// SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	// SDL_RenderPoints(g_renderer, g_pointbuf.data(), g_pointbuf.size());
	writeColouredPointCommands(g_colourbuf, g_pointbuf);

	SDL_RenderPresent(g_renderer);
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	return;
}