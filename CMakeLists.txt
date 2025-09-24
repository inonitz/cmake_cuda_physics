cmake_minimum_required (VERSION 3.15..4.0)
project(
    testing_cuda_with_cmake
    VERSION 0.00.1
    DESCRIPTION "Testing CUDA with CMAKE (Executable)"
    LANGUAGES ASM C CXX CUDA
)
set(EXECUTABLE_NAME ${PROJECT_NAME})


# Taken from: https://cliutils.gitlab.io/modern-cmake/chapters/projects/submodule.html
function(UPDATE_SUBMODULES_ON_CMAKE_CONFIGURE SUBMODULE_CMAKE_TXT_PATH)
    find_package(Git QUIET)
    if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
        # Update submodules as needed
        option(GIT_SUBMODULE "Check submodules during build" ON)
        if(GIT_SUBMODULE)
            message(STATUS "Submodule update")
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                            RESULT_VARIABLE GIT_SUBMOD_RESULT
            )
            if(NOT GIT_SUBMOD_RESULT EQUAL "0")
                message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
            endif()
        endif()
    endif()

    if(NOT EXISTS ${PROJECT_SOURCE_DIR}/${SUBMODULE_CMAKE_TXT_PATH})
        message(${PROJECT_SOURCE_DIR}/${SUBMODULE_CMAKE_TXT_PATH})
        message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
    endif()
endfunction() 


update_submodules_on_cmake_configure("dependencies/SDL/CMakeLists.txt")




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
set(CMAKE_CUDA_STANDARD_REQUIRED  ON)
set(CMAKE_CXX_EXTENSIONS          OFF)
if(NOT DEFINED CMAKE_C_STANDARD)
    set(CMAKE_C_STANDARD 11)
endif()
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()
if(NOT DEFINED CMAKE_CUDA_STANDARD)
    set(CMAKE_CUDA_STANDARD 17)
endif()


if(NOT DEFINED CMAKE_PROJECT_DIRECTORY)
    set(CMAKE_PROJECT_DIRECTORY "${CMAKE_SOURCE_DIR}/projects")
endif()



find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_ASM_COMPILER_LAUNCHER  "${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER    "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER  "${CCACHE_PROGRAM}")
    set(CMAKE_CUDA_COMPILER_LAUNCHER "${CCACHE_PROGRAM}") # CMake 3.9+
endif()


OPTION(MEASURE_PERFORMANCE_TIMEOUT 
    "Timeout the program after a certain amount of iterations" 
    ON
)

find_package(Vulkan REQUIRED)
add_subdirectory(projects/program)
add_subdirectory(projects/util2)
add_subdirectory(dependencies/SDL)
add_subdirectory(dependencies/SDL_shadercross)