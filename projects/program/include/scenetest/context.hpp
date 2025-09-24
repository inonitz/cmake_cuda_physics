#ifndef __SCENE_TEST_APP_CONTEXT_DEFINITION_HEADER__
#define __SCENE_TEST_APP_CONTEXT_DEFINITION_HEADER__
#include <vector>
#include <SDL3/SDL.h>
#include <util2/vec2.hpp>
#include "scenetest/shader.hpp"


class alignsz(64) AppContext 
{
public:
	[[nodiscard]] auto create(
		bool debug_mode,
		u16  windowWidth,
		u16  windowHeight
	) -> SDL_AppResult;


	[[nodiscard]] SDL_AppResult update();

	void destroy();


	[[nodiscard]] auto getFrameCount() const -> u64 {
		return m_frameCount;
	}

private:
	void prepareVertexBuffers();
	void createGraphicsPipeline();
	void createComputePipeline();
	void incrementTick();

private:
	u64 m_lasttime   = 0;
	u64 m_currtime   = 0;
	u32 m_timeDelta  = 0;
	u32 m_frameCount = 0;
	SDL_GPUDevice*           m_gpudevice        = nullptr;
	SDL_GPUGraphicsPipeline* m_gfxpipeline      = nullptr;
	SDL_GPUTexture*			 m_swapchainTexture = nullptr;
	u32 					 m_swapchainWidth   = 0;
	u32 					 m_swapchainHeight  = 0;
	SDL_GPUCommandBuffer*	 m_cmdbuf		    = nullptr;
	SDL_GPURenderPass* 		 m_renderPass	    = nullptr;
	SDL_GPUTransferBuffer*   m_transferBuffer   = nullptr;
	SDL_GPUBuffer*           m_vertexBuffer     = nullptr;
	SDL_GPUComputePipeline*  m_cmppipeline      = nullptr;
	SDL_Window*              m_window           = nullptr;
	vertexSDLShader		     m_shader0;
	fragmentSDLShader	     m_shader1;
	computeSDLShader		 m_shader2;
	std::array<const char*, 3> 		m_shaderPaths;
	std::vector<util2::math::vec3f> m_spherePrimitiveVertices;
};


#endif /* __SCENE_TEST_APP_CONTEXT_DEFINITION_HEADER__ */
