#include "scenetest/shader.hpp"
#include "check.hpp"
#include <util2/C/ifcrash2.h>


template<
    SDL_ShaderCross_ShaderStage t_stage
>
SDL_ShaderCross_SPIRV_Info* SDLShader<t_stage>::create(
    SDL_GPUDevice* 				   device, 
    const char*    				   shaderfile,
    shaderc::CompileOptions const& compilationFlags
) {
    static GLSLShader glsl_to_spirv_translator;
    ShaderSourceBuffer shaderSpirvCode;


    u32   shaderCodeSize = 0;
    u8*   shaderCodePtr  = nullptr;
    auto* shaderinfo     = __scast(SDL_ShaderCross_SPIRV_Info*, 
        SDL_malloc(sizeof(SDL_ShaderCross_SPIRV_Info))
    );

    
    glsl_to_spirv_translator.compileGLSLToSpirvBinary(
        shaderSpirvCode,
        shaderfile,
        getShaderc_ShaderKind(),
        compilationFlags
    );
    shaderCodeSize = shaderSpirvCode.size();
    shaderCodePtr  = __rcast(u8*, shaderSpirvCode.begin());
    *shaderinfo = (SDL_ShaderCross_SPIRV_Info){
        .bytecode      = shaderCodePtr,
        .bytecode_size = shaderCodeSize,
        .entrypoint    = "main",
        .shader_stage  = t_stage
    };


    m_device = device; /* shouldn't be modified ever again */
    if constexpr ( shaderTypeIsCompute() ) {
        m_reflectmeta = SDL_ShaderCross_ReflectComputeSPIRV(shaderCodePtr, shaderCodeSize, SDL_PropertiesID{});
        m_shader      = SDL_ShaderCross_CompileComputePipelineFromSPIRV(device, shaderinfo, m_reflectmeta, 0);
    } else {
        m_reflectmeta = SDL_ShaderCross_ReflectGraphicsSPIRV(shaderCodePtr, shaderCodeSize, SDL_PropertiesID{});
        m_shader = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(device, shaderinfo, &m_reflectmeta->resource_info, 0);
    }

    shaderSpirvCode.destroy();
    return shaderinfo;
}


template<
    SDL_ShaderCross_ShaderStage t_stage
>
void SDLShader<t_stage>::destroy() {
    SDL_free(m_reflectmeta);

    if constexpr ( shaderTypeIsCompute() ) {
        SDL_ReleaseGPUComputePipeline(m_device, m_shader);
    } else {
        SDL_ReleaseGPUShader(m_device, m_shader);
    }
}


template class SDLShader<SDL_SHADERCROSS_SHADERSTAGE_VERTEX>;
template class SDLShader<SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT>;
template class SDLShader<SDL_SHADERCROSS_SHADERSTAGE_COMPUTE>;