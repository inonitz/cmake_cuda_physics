#include "scenetest/spirv.hpp"
#include <util2/C/file.h>
#include <util2/C/ifcrash2.h>
#include <util2/bufferptr.hpp>


void GLSLShader::create(
    const char*             filepath, 
    shaderc_shader_kind     shaderType,
    shaderc::CompileOptions const& flags
) {
    ShaderSourceBuffer    bigEnoughBuffer;
    ShaderStageDescriptor info{shaderType, {}, ShaderSourceBuffer{}, flags, filepath };

    m_filepath        = filepath;
    m_shaderStageType = shaderType;
    m_shaderArgs      = new shaderc::CompileOptions{flags};
    
    
    loadFile(m_filepath, m_glslSource);
    return;
}


void GLSLShader::destroy()
{
    m_filepath = "";
    m_shaderStageType = shaderc_vertex_shader;
    
    delete m_shaderArgs;
    m_shaderArgs = nullptr;
    return;
}


void GLSLShader::compile()
{
    ShaderSourceBuffer    outputBuffer;
    ShaderStageDescriptor info{m_shaderStageType, {}, {}, *m_shaderArgs, m_filepath };


    info.m_genericSource.create(m_glslSource);
    preprocessGLSL(outputBuffer, info);

    info.m_genericSource.destroy();
    outputBuffer.moveTo(info.m_genericSource);

    compilePostProcessedGLSLtoSpirvAssembly(
        info.m_genericSource,
        info
    );
    info.m_genericSource.destroy();
    outputBuffer.moveTo(info.m_genericSource);

    compileSpirvAssemblyToSpirvBinary(
        info.m_genericSource,
        info
    );
    info.m_genericSource.destroy();

    /* Destination buffer*/
    outputBuffer.moveTo(m_spirvBinary);
    return;
}



void GLSLShader::reloadFileContents()
{
    loadFile(m_filepath, m_glslSource);
    return;
}


// void GLSLShader::preprocessGLSL()
// {

//     //compile
//         compiler.PreprocessGlsl(info.source.data(), info.source.size(), info.kind, info.fileName, info.options);
//     if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
//         logger->print(result.GetErrorMessage());
//     }

//     //copy result into info for next compilation operation
//     const char* src = result.cbegin();
//     size_t newSize = result.cend() - src;
//     info.source.resize(newSize);
//     memcpy(info.source.data(), src, newSize);

//     //Log output for fun
//     logger->print("---- Preprocessed GLSL source code ----");
//     std::string output = { info.source.data(), info.source.data() + info.source.size() };
//     logger->print(output);
// }


void GLSLShader::loadFile(
    std::string_view const& filepath,
    ShaderSourceBuffer&     sourceOutput
) {
    u32   size = 0;
    char* shaderLoadedFile = NULL;
    
    util2_load_file(filepath.data(), &size, NULL);
    
    shaderLoadedFile = __scast(char*, malloc(size) );
    util2_load_file(filepath.data(), &size, shaderLoadedFile);

    sourceOutput.create(shaderLoadedFile, size);
    return;
}


template<
    GLSLShader::ShaderCompilationStage 
    stage
>
void GLSLShader::compileGenericShaderStage(
    ShaderSourceBuffer&          output,
    ShaderStageDescriptor const& info
) {
    ShaderStageResultType<stage> result;
    shaderc::Compiler            compiler;
    u32                          outputSize   = 0;
    char*                        outputBuffer = NULL;

    if constexpr (stage == ShaderCompilationStage::PREPROCESS_GLSL) 
    {
        result = compiler.PreprocessGlsl(
            info.m_genericSource.begin(), 
            info.m_genericSource.size(), 
            info.m_stage, 
            nullptr, 
            info.m_compileFlags
        );
        ifcrashfmt(result.GetCompilationStatus() != shaderc_compilation_status_success,
            "Unsuccessful pre-processing of glsl file '%s' ->\n    %s",
            info.m_filepath.data(),
            result.GetErrorMessage().data()
        );
    }
    
    else if constexpr (stage == ShaderCompilationStage::SPIRV_ASSEMBLY) 
    {
        result = compiler.CompileGlslToSpvAssembly(
            info.m_genericSource.data(),
            info.m_genericSource.size(),
            info.m_stage,
            nullptr,
            "main",
            info.m_compileFlags
        );
        ifcrashfmt(result.GetCompilationStatus() != shaderc_compilation_status_success,
            "Unsuccessful GLSL -> SPV Assembly Compilation for file '%s' ->\n    %s",
            info.m_filepath.data(),
            result.GetErrorMessage().data()
        );
    }

    else if constexpr (stage == ShaderCompilationStage::SPIRV_BINARY) 
    {
        result = compiler.AssembleToSpv(
            info.m_genericSource.data(), 
            info.m_genericSource.size(), 
            info.m_compileFlags
        );
        ifcrashfmt(result.GetCompilationStatus() != shaderc_compilation_status_success,
            "Unsuccessful SPV Assembly -> SPV Binary Compilation for file '%s' ->\n    %s",
            info.m_filepath.data(),
            result.GetErrorMessage().data()
        );
    } 

    else {
        ifcrashfmt(true, "Invalid ShaderCompilationStage Supplied\n");
        return;
    }


    outputSize   = result.end() - result.begin();
    outputBuffer = __scast(char*, malloc(outputSize) );
    output.create(outputBuffer, outputSize);
    output.copyFrom(result.begin());
    return;
}


void GLSLShader::preprocessGLSL(
    ShaderSourceBuffer&          glslOutput,
    ShaderStageDescriptor const& info
) {    
    // shaderc::PreprocessedSourceCompilationResult result;
    // shaderc::Compiler compiler;
    // u32               outputSize   = 0;
    // char*             outputBuffer = NULL;


    // result = compiler.PreprocessGlsl(
    //     info.m_genericSource.begin(), 
    //     info.m_genericSource.size(), 
    //     info.m_stage, 
    //     nullptr, 
    //     info.m_compileFlags
    // );
    // ifcrashfmt(result.GetCompilationStatus() != shaderc_compilation_status_success,
    //     "Unsuccessful pre-processing of glsl file '%s' ->\n    %s",
    //     info.m_filepath.data(),
    //     result.GetErrorMessage().data()
    // );


    // outputSize = result.end() - result.begin();
    // outputBuffer = __scast(char*, malloc(outputSize) );
    // glslOutput.create(outputBuffer, outputSize);
    // glslOutput.copy(result.begin());


    compileGenericShaderStage<ShaderCompilationStage::PREPROCESS_GLSL>(
        glslOutput,
        info
    );
    return;
}


void GLSLShader::compilePostProcessedGLSLtoSpirvAssembly(
    ShaderSourceBuffer&          spirvOutput,
    ShaderStageDescriptor const& info
) {
    // shaderc::AssemblyCompilationResult result;
    // shaderc::Compiler compiler;
    // u32               outputSize   = 0;
    // char*             outputBuffer = NULL;


    // result = compiler.CompileGlslToSpvAssembly(
    //     info.m_genericSource.data(),
    //     info.m_genericSource.size(),
    //     info.m_stage,
    //     nullptr,
    //     entrypointName.data(),
    //     info.m_compileFlags
    // );
    // ifcrashfmt(result.GetCompilationStatus() != shaderc_compilation_status_success,
    //     "Unsuccessful GLSL -> SPV Assembly Compilation for file '%s' ->\n    %s",
    //     info.m_filepath.data(),
    //     result.GetErrorMessage().data()
    // );

    // outputSize = result.end() - result.begin();
    // outputBuffer = __scast(char*, malloc(outputSize) );
    // spirvOutput.create(outputBuffer, outputSize);
    // spirvOutput.copy(result.begin());


    compileGenericShaderStage<ShaderCompilationStage::SPIRV_ASSEMBLY>(
        spirvOutput,
        info
    );
    return;
}


void GLSLShader::compileSpirvAssemblyToSpirvBinary(
    ShaderSourceBuffer&          spirvBinaryOutput,
    ShaderStageDescriptor const& postAssemblyInfo
) {
    compileGenericShaderStage<ShaderCompilationStage::SPIRV_ASSEMBLY>(
        spirvBinaryOutput,
        postAssemblyInfo
    );
}


void GLSLShader::compileGLSLToSpirvBinary(
    ShaderSourceBuffer&            spirvBinaryOutput,
    const char*                    filepath, 
    shaderc_shader_kind            shaderType,
    shaderc::CompileOptions const& flags
) {
    ShaderSourceBuffer    outputBuffer;
    ShaderStageDescriptor info{shaderType, {}, {}, flags, filepath };


    loadFile(filepath, spirvBinaryOutput);
    
    info.m_genericSource.create(spirvBinaryOutput);
    preprocessGLSL(outputBuffer, info);

    info.m_genericSource.destroy();
    outputBuffer.moveTo(info.m_genericSource);

    compilePostProcessedGLSLtoSpirvAssembly(
        info.m_genericSource,
        info
    );
    info.m_genericSource.destroy();
    outputBuffer.moveTo(info.m_genericSource);

    compileSpirvAssemblyToSpirvBinary(
        info.m_genericSource,
        info
    );
    info.m_genericSource.destroy();

    /* Destination buffer*/
    outputBuffer.moveTo(spirvBinaryOutput);
    return;
}