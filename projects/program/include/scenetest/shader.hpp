#ifndef __SCENE_TEST_SHADER_ABSTRACTION_DEFINITION_HEADER__
#define __SCENE_TEST_SHADER_ABSTRACTION_DEFINITION_HEADER__
#include <SDL3_shadercross/SDL_shadercross.h>
#include <util2/type_trait.hpp>


template<
	SDL_ShaderCross_ShaderStage shaderStage
>
class SDLShader
{
private:
	static constexpr auto shaderTypeIsCompute() -> bool {
		return shaderStage == SDL_ShaderCross_ShaderStage::SDL_SHADERCROSS_SHADERSTAGE_COMPUTE;
	} 

	using metadataType = typename util2::type_trait::conditional_operator<shaderTypeIsCompute(), 
		SDL_ShaderCross_ComputePipelineMetadata, /* we can only create a compute pipeline, not a compute shader. so thats that */
		SDL_ShaderCross_GraphicsShaderMetadata
	>::type;
	using shaderType = typename util2::type_trait::conditional_operator<shaderTypeIsCompute(), 
		SDL_GPUComputePipeline,
		SDL_GPUShader
	>::type;


public:
	SDLShader()  = default;
	~SDLShader() = default;
	SDLShader(const SDLShader&) = delete;
	SDLShader(SDLShader&&     ) = delete;
	SDLShader& operator=(const SDLShader& ) = delete;
	SDLShader& operator=(const SDLShader&&) = delete;


	auto create(
		SDL_GPUDevice* device, 
		const char*    shaderfile
	) -> SDL_ShaderCross_SPIRV_Info*;

	void destroy();


	auto getMetadata() const -> const metadataType* {
		return m_reflectmeta;
	}
	auto getShaderHandle() const -> shaderType* { 
		return m_shader; 
	}

	
private:
	SDL_GPUDevice* m_device{nullptr};
	metadataType*  m_reflectmeta{nullptr};
	shaderType*    m_shader{nullptr};
};


using vertexSDLShader   = SDLShader<SDL_SHADERCROSS_SHADERSTAGE_VERTEX  >;
using fragmentSDLShader = SDLShader<SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT>;
using computeSDLShader  = SDLShader<SDL_SHADERCROSS_SHADERSTAGE_COMPUTE >;


#endif /* __SCENE_TEST_SHADER_ABSTRACTION_DEFINITION_HEADER__ */
