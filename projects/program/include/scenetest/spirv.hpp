#ifndef __SHADER_RUNTIME_COMPILATION_DEFINITION_HEADER__
#define __SHADER_RUNTIME_COMPILATION_DEFINITION_HEADER__
#include <shaderc/shaderc.hpp>
#include <util2/bufferptr.hpp>


using ShaderSourceBuffer = util2::ManagedMallocPointer<char>;


struct ShaderStageDescriptor {
    shaderc_shader_kind     m_stage;
    uint8_t                 m_reserved[4];
    ShaderSourceBuffer      m_genericSource;
    shaderc::CompileOptions m_compileFlags;
    std::string_view        m_filepath;
};



class GLSLShader 
{
public:
    void create(
        const char*                    filepath, 
        shaderc_shader_kind            shaderType,
        shaderc::CompileOptions const& flags
    );
    void destroy();
    
    void compile();
    void reloadFileContents();


    static void loadFile(
        std::string_view const& filepath,
        ShaderSourceBuffer&     sourceOutput
    );

    static void preprocessGLSL(
        ShaderSourceBuffer&          glslOutput,
        ShaderStageDescriptor const& info
    );

    static void compilePostProcessedGLSLtoSpirvAssembly(
        ShaderSourceBuffer&          spirvOutput,
        ShaderStageDescriptor const& postGLSLProcessInfo
    );

    static void compileSpirvAssemblyToSpirvBinary(
        ShaderSourceBuffer&          spirvBinaryOutput,
        ShaderStageDescriptor const& postAssemblyInfo
    );

    static void compileGLSLToSpirvBinary(
        ShaderSourceBuffer&            spirvBinaryOutput,
        const char*                    filepath, 
        shaderc_shader_kind            shaderType,
        shaderc::CompileOptions const& flags
    );


private:
    enum class ShaderCompilationStage : u8 {
        PREPROCESS_GLSL = 0,
        SPIRV_ASSEMBLY  = 1,
        SPIRV_BINARY    = 2
    };


    template<ShaderCompilationStage stage>
    static void compileGenericShaderStage(
        ShaderSourceBuffer&          genericOutput,
        ShaderStageDescriptor const& genericInfo
    );

    template<> void compileGenericShaderStage<ShaderCompilationStage::PREPROCESS_GLSL>(
        ShaderSourceBuffer&          genericOutput,
        ShaderStageDescriptor const& genericInfo
    );
    template<> void compileGenericShaderStage<ShaderCompilationStage::SPIRV_ASSEMBLY>(
        ShaderSourceBuffer&          genericOutput,
        ShaderStageDescriptor const& genericInfo
    );
    template<> void compileGenericShaderStage<ShaderCompilationStage::SPIRV_BINARY>(
        ShaderSourceBuffer&          genericOutput,
        ShaderStageDescriptor const& genericInfo
    );


    template<ShaderCompilationStage stage>
    using ShaderStageResultType = 
        std::conditional_t< 
            (stage == ShaderCompilationStage::PREPROCESS_GLSL),
            shaderc::PreprocessedSourceCompilationResult, /* if preprocess */
            std::conditional_t<
                (stage == ShaderCompilationStage::SPIRV_ASSEMBLY),
                shaderc::AssemblyCompilationResult, /* if spirv assembly */
                shaderc::SpvCompilationResult       /* can only be spirv binary */
            >
        >;

private:
    struct ShaderCompilationStages {
        ShaderSourceBuffer m_shaderSource;
        ShaderSourceBuffer m_preprocessedGLSL;
        ShaderSourceBuffer m_spirvAssembly;
        ShaderSourceBuffer m_spirvBinary;
    };
    const char*              m_filepath{};
    shaderc_shader_kind      m_shaderStageType{};
    shaderc::CompileOptions* m_shaderArgs{};
    ShaderSourceBuffer       m_glslSource;
    ShaderSourceBuffer       m_spirvBinary;
};




#endif /* __SHADER_RUNTIME_COMPILATION_DEFINITION_HEADER__ */