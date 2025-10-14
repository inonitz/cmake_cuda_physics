#include "scenetest/context.hpp"
#include <vector>
#include <util2/vec2.hpp>
#include <SDL3_shadercross/SDL_shadercross.h>
#include "scenetest/sphere.hpp"
#include "scenetest/camera.hpp"
#include "check.hpp"








auto AppContext::create(
	bool debug_mode,
	u16  windowWidth,
	u16  windowHeight
) -> SDL_AppResult
{
    u8 status = true;


    m_window    = SDL_CreateWindow("", windowWidth, windowHeight, SDL_WINDOW_RESIZABLE);
	m_gpudevice = SDL_CreateGPUDevice(SDL_ShaderCross_GetSPIRVShaderFormats(), debug_mode, NULL);
    status      = SDL_ClaimWindowForGPUDevice(m_gpudevice, m_window);

	SDLCHECK(m_window    == nullptr, "Failure to create Application window\n");
	SDLCHECK(m_gpudevice == nullptr, "Failure to create GPU Device\n");
	SDLCHECK(status      != true,    "Failure to claim current window for Graphics Device\n");


	m_io.create();
	m_lasttime = SDL_GetTicks();
	m_shaderPaths[0] = "shaders/bin/vert.spv";
	m_shaderPaths[1] = "shaders/bin/frag.spv";
	m_shaderPaths[2] = "shaders/bin/comp.spv";

	prepareVertexBuffers();
	createGraphicsPipeline();
	createComputePipeline();
	return SDL_APP_CONTINUE;
}


void AppContext::destroy()
{
	SDL_ReleaseGPUGraphicsPipeline(m_gpudevice, m_gfxpipeline);
	m_shader2.destroy();
	m_shader1.destroy();
	m_shader0.destroy();


	SDL_ReleaseGPUTransferBuffer(m_gpudevice, m_transferBuffer);
	SDL_ReleaseGPUBuffer(m_gpudevice, m_vertexBuffer);
	SDL_DestroyGPUDevice(m_gpudevice);
	SDL_DestroyWindow(m_window);
	m_io.destroy();
    return;
}


auto AppContext::inputUpdate(void* appstate, SDL_Event* event) -> SDL_AppResult {
	auto status = SDL_APP_CONTINUE;

	switch(event->type) {
		case SDL_EVENT_QUIT:
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			status = SDL_APP_SUCCESS;
		break;
		default:
			m_io.onUpdate(appstate, *event);
			m_camera->update(__scast(f32, m_currtime));
			status = SDL_APP_CONTINUE;
		break;
	}
	return status;
}


auto AppContext::renderUpdate() -> SDL_AppResult
{
	constexpr SDL_FColor clearScreenColour{
		0.74281051f, 0.6502934f, 0.97186973f, 1.0f
	};
	SDL_GPUColorTargetInfo colorTargetInfo{};
	SDL_GPUBufferBinding bufferBinding{
		.buffer = m_vertexBuffer,
		.offset = 0
	};
	

	m_cmdbuf = SDL_AcquireGPUCommandBuffer(m_gpudevice);
	
	SDLCHECK(m_cmdbuf == nullptr, "Failure to acquire current Command Buffer\n");
	SDLCHECK(!SDL_WaitAndAcquireGPUSwapchainTexture(
			m_cmdbuf,
			m_window,
			&m_swapchainTexture,
			&m_swapchainSize[0],
			&m_swapchainSize[1]
		),
		"Failure to acquire current swapchain-texture\n"
	);


	if (m_swapchainTexture != nullptr)
	{
		colorTargetInfo.texture 	= m_swapchainTexture;
		colorTargetInfo.clear_color = clearScreenColour;
		colorTargetInfo.load_op 	= SDL_GPU_LOADOP_CLEAR;
		colorTargetInfo.store_op 	= SDL_GPU_STOREOP_STORE;
		
		m_renderPass = SDL_BeginGPURenderPass(m_cmdbuf, &colorTargetInfo, 1, nullptr);
		
		SDL_BindGPUGraphicsPipeline(m_renderPass, m_gfxpipeline);
		SDL_BindGPUVertexBuffers(m_renderPass, 0, &bufferBinding, 1);


		SDL_PushGPUVertexUniformData(m_cmdbuf, 3, &m_camera->getProjection(), sizeof(util2::math::mat4f));
		SDL_PushGPUVertexUniformData(m_cmdbuf, 4, &m_camera->getView(), 	  sizeof(util2::math::mat4f));
		// SDL_PushGPUVertexUniformData(m_cmdbuf, 1, &m_camera->getView(), 	  sizeof(util2::math::mat4f));
		SDL_DrawGPUPrimitives(m_renderPass, m_spherePrimitiveVertices.size(), 1, 0, 0);
		
		SDL_EndGPURenderPass(m_renderPass);
	}


	SDL_SubmitGPUCommandBuffer(m_cmdbuf);
	return SDL_APP_CONTINUE;
}




void AppContext::prepareVertexBuffers()
{
	SDL_GPUTransferBufferCreateInfo transferBufferInfo{};
    SDL_GPUBufferCreateInfo 		vertexBufferInfo{};
	u32 							bufferSizeBytes = 0;
	void*							cpuMappedTransferBuf = nullptr;
	SDL_GPUCopyPass* 				copyPass = nullptr;
	SDL_GPUTransferBufferLocation 	src{};
	SDL_GPUBufferRegion 		  	dest{};

	generateSphere(16, 16, 1, m_spherePrimitiveVertices);
	bufferSizeBytes  = __scast(u32, m_spherePrimitiveVertices.size() * sizeof(util2::math::vec3f) );
	

	vertexBufferInfo = {
		.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
		.size  = bufferSizeBytes,
		.props = 0
	};
    transferBufferInfo = {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size  = bufferSizeBytes,
		.props = 0
	};
    m_vertexBuffer   	 = SDL_CreateGPUBuffer(m_gpudevice, &vertexBufferInfo);
	m_transferBuffer 	 = SDL_CreateGPUTransferBuffer(m_gpudevice, &transferBufferInfo);
	cpuMappedTransferBuf = SDL_MapGPUTransferBuffer(
		m_gpudevice, 
		m_transferBuffer, 
		false
	);

	/* Valid State checking */
	ifcrash(
		m_vertexBuffer   == nullptr || 
		m_transferBuffer == nullptr || 
		cpuMappedTransferBuf == nullptr
	);


	/* 1. Copy to transfer buffer */
    SDL_memcpy(cpuMappedTransferBuf, (void*)m_spherePrimitiveVertices.data(), bufferSizeBytes);
    
	SDL_UnmapGPUTransferBuffer(m_gpudevice, m_transferBuffer);
	cpuMappedTransferBuf = nullptr;


	/* 2. Initiate a copy-pass to the actual GPU Buffer */
	m_cmdbuf = SDL_AcquireGPUCommandBuffer(m_gpudevice);
    copyPass = SDL_BeginGPUCopyPass(m_cmdbuf);

    src  = { m_transferBuffer, 0 };
    dest = { m_vertexBuffer,   0, bufferSizeBytes };
    SDL_UploadToGPUBuffer(copyPass, &src, &dest, false); /* Actual memcpy */

    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(m_cmdbuf);
	return;
}


void AppContext::createGraphicsPipeline()
{
	SDL_GPUVertexBufferDescription vertexBufferDescriptions[1];
	SDL_GPUVertexAttribute 		   vertexAttributes[1];
	SDL_GPUVertexInputState 	   vertexInputs{};
	
	SDL_GPURasterizerState			  rasterizerStateInfo{};
	SDL_GPUMultisampleState			  multiSamplerStateInfo{};
	SDL_GPUDepthStencilState		  stencilStateInfo{};
    
	SDL_GPUColorTargetDescription 	  colorTargetDescriptions[1];
	SDL_GPUGraphicsPipelineTargetInfo pipelineTargetInfo{};
	
	SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo{};

	shaderc::CompileOptions vertexShaderCompileOptions;
	shaderc::CompileOptions fragmentShaderCompileOptions;


	vertexBufferDescriptions[0] = {
		.slot 			    = 0,
		.pitch 			    = sizeof(util2::math::vec3f),
		.input_rate 	    = SDL_GPU_VERTEXINPUTRATE_VERTEX,
		.instance_step_rate = 0
	};
	vertexAttributes[0] =  {
		.location 	 = 0,
		.buffer_slot = 0,
		.format 	 = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
		.offset 	 = 0
	};
	vertexInputs = {
		&vertexBufferDescriptions[0],
		1,
		&vertexAttributes[0],
		1
	};


	colorTargetDescriptions[0] = {};
    colorTargetDescriptions[0].format = SDL_GetGPUSwapchainTextureFormat(m_gpudevice, m_window);
	pipelineTargetInfo = {
		.color_target_descriptions = &colorTargetDescriptions[0], 
		.num_color_targets		   = 1, 	
		.depth_stencil_format	   = SDL_GPU_TEXTUREFORMAT_INVALID, 
		.has_depth_stencil_target  = false, 
		.padding1				   = 0,
		.padding2				   = 0, 	
		.padding3				   = 0
	};

	vertexShaderCompileOptions
	fragmentShaderCompileOptions
	m_shader0.create(m_gpudevice, m_shaderPaths[0]);
	m_shader1.create(m_gpudevice, m_shaderPaths[1]);


	pipelineCreateInfo = (SDL_GPUGraphicsPipelineCreateInfo){
		.vertex_shader		 = m_shader0.getShaderHandle(),
		.fragment_shader	 = m_shader1.getShaderHandle(),
		.vertex_input_state	 = vertexInputs,
		.primitive_type		 = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
		.rasterizer_state	 = rasterizerStateInfo,
		.multisample_state	 = multiSamplerStateInfo,
		.depth_stencil_state = stencilStateInfo,
		.target_info		 = pipelineTargetInfo,
		.props				 = 0
	};
	m_gfxpipeline = SDL_CreateGPUGraphicsPipeline(m_gpudevice, &pipelineCreateInfo);


	ifcrash(m_gfxpipeline == nullptr);
	return;
}


void AppContext::createComputePipeline()
{
	static const shaderc::CompileOptions defaultGlslCompileOptions{}; /* TOOD SET */
	m_shader2.create(m_gpudevice, m_shaderPaths[2], defaultGlslCompileOptions);
	m_cmppipeline = m_shader2.getShaderHandle();

	ifcrash(m_cmppipeline == nullptr);
	return;
}


void AppContext::incrementTick()
{
	m_currtime  = SDL_GetTicksNS();
	m_timeDelta = (m_currtime - m_lasttime);
	m_lasttime  = m_currtime;
	++m_frameCount;
    return;
}