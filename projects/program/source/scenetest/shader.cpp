#include "scenetest/shader.hpp"
#include "check.hpp"
#include <util2/C/ifcrash2.h>


template<
    SDL_ShaderCross_ShaderStage t_stage
>
SDL_ShaderCross_SPIRV_Info* SDLShader<t_stage>::create(
    SDL_GPUDevice* 				device, 
    const char* 				shaderfile
) {
    size_t shadercodesize = 0; 
    u8*    shadercode  	  = nullptr;
    auto*  shaderinfo     = __scast(SDL_ShaderCross_SPIRV_Info*, 
        SDL_malloc(sizeof(SDL_ShaderCross_SPIRV_Info))
    );

    
    shadercode = __scast(u8*, SDL_LoadFile(shaderfile, &shadercodesize) );
    ifcrashdo(shadercode == nullptr, {
        SDLSTATUS(true, "Failure to load file using SDL_LoadFile\n");
    });
    *shaderinfo = (SDL_ShaderCross_SPIRV_Info){
        .bytecode      = shadercode,
        .bytecode_size = shadercodesize,
        .entrypoint    = "main",
        .shader_stage  = t_stage
    };


    m_device = device; /* shouldn't be modified ever again */
    if constexpr ( shaderTypeIsCompute() ) {
        m_reflectmeta = SDL_ShaderCross_ReflectComputeSPIRV(shadercode, shadercodesize, SDL_PropertiesID{});
        m_shader      = SDL_ShaderCross_CompileComputePipelineFromSPIRV(device, shaderinfo, m_reflectmeta, 0);
    } else {
        m_reflectmeta = SDL_ShaderCross_ReflectGraphicsSPIRV(shadercode, shadercodesize, SDL_PropertiesID{});
        m_shader = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(device, shaderinfo, &m_reflectmeta->resource_info, 0);
    }

    SDL_free(shadercode);
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