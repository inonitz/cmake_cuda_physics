cmake_minimum_required (VERSION 3.15..4.0)
project(
    hydrogen_atom_electron_cloud
    VERSION 0.00.1
    DESCRIPTION "Electron Cloud - Hydrogen Atom (3D) (Executable)"
    LANGUAGES ASM C CXX
)
set(EXECUTABLE_NAME ${PROJECT_NAME})


# Vulkan Search
if (DEFINED VULKAN_SDK_PATH)
    set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include") # 1.1 Make sure this include path is correct
    set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib") # 1.2 Make sure lib path is correct
    set(Vulkan_FOUND "True")
else()
    find_package(Vulkan REQUIRED COMPONENTS 
        glslang 
        shaderc_combined
    )
    message(STATUS "Found Vulkan: '$ENV{VULKAN_SDK}'")
endif()
if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find Vulkan library!")
else()
    message(STATUS "Using Vulkan Library at '${Vulkan_LIBRARIES}'")
endif()


# spirv_cross_c_shared - required for sdl_shadercross
find_package(spirv_cross_c_shared CONFIG REQUIRED)
if (spirv_cross_c_shared_FOUND)
    message(STATUS "Found SPIRV-Cross C API!")
else()
    message(STATUS "Could not find SPIRV-Cross C API! :(")
endif()


# shaderc - required for runtime compilation of glsl code
# find_package(shaderc_combined CONFIG REQUIRED)
if (Vulkan_shaderc_combined_FOUND)
    message(STATUS "Found Vulkan ShaderC API!")
else()
    message(STATUS "Could not find Vulkan ShaderC API! :(")
endif()



# Taken from: https://cliutils.gitlab.io/modern-cmake/chapters/projects/submodule.html
function(UPDATE_SUBMODULES_ON_CMAKE_CONFIGURE SUBMODULE_CMAKE_TXT_PATH)
    find_package(Git QUIET)
    if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
        # Update submodules as needed
        option(GIT_SUBMODULE "Check submodules during build" ON)
        if(GIT_SUBMODULE)
            message(STATUS "Submodule update") # see: https://stackoverflow.com/a/21195182
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --remote
                            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                            RESULT_VARIABLE GIT_SUBMOD_RESULT
            )
            if(NOT GIT_SUBMOD_RESULT EQUAL "0")
                message(FATAL_ERROR "git submodule update --init --recursive --remote failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
            endif()
        endif()
    endif()

    if(NOT EXISTS ${PROJECT_SOURCE_DIR}/${SUBMODULE_CMAKE_TXT_PATH})
        message(${PROJECT_SOURCE_DIR}/${SUBMODULE_CMAKE_TXT_PATH})
        message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
    endif()
endfunction() 


update_submodules_on_cmake_configure("dependencies/SDL/CMakeLists.txt")
update_submodules_on_cmake_configure("dependencies/SDL_shadercross/CMakeLists.txt")




# Using <config> will generate in a multi-generator-dependant way, which is undesirable
# I'd rather choose it myself using a script.
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")

# makes sure all dynamic/static/executable objects sit in the same place
set(CMAKE_CUSTOM_BUILD_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CUSTOM_BUILD_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CUSTOM_BUILD_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CUSTOM_BUILD_OUTPUT_DIRECTORY})

# prevent installing to system directories. 
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")




set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_STANDARD_REQUIRED   ON)
# set(CMAKE_CUDA_STANDARD_REQUIRED  ON)
set(CMAKE_CXX_EXTENSIONS          OFF)
if(NOT DEFINED CMAKE_C_STANDARD)
    set(CMAKE_C_STANDARD 11)
endif()
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()
# if(NOT DEFINED CMAKE_CUDA_STANDARD)
#     set(CMAKE_CUDA_STANDARD 17)
# endif()


# default build type unless user defined
if(NOT DEFINED CMAKE_BUILD_TYPE)
    # set(cmake_build_type_undefined 1)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "CMake build type" FORCE)
endif()


if(NOT DEFINED CMAKE_PROJECT_DIRECTORY)
    set(CMAKE_PROJECT_DIRECTORY "${CMAKE_SOURCE_DIR}/projects")
endif()



find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_ASM_COMPILER_LAUNCHER  "${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER    "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER  "${CCACHE_PROGRAM}")
    # set(CMAKE_CUDA_COMPILER_LAUNCHER "${CCACHE_PROGRAM}") # CMake 3.9+
endif()


OPTION(MEASURE_PERFORMANCE_TIMEOUT 
    "Timeout the program after a certain amount of iterations" 
    ON
)


add_subdirectory(projects/program)
add_subdirectory(projects/util2)
add_subdirectory(dependencies/SDL)
add_subdirectory(dependencies/SDL_shadercross)