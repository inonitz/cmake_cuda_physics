include(CMakePrintHelpers)
include(FeatureSummary)


function(COMPILE_AND_COPY_SHADERS_TO_BINARY_DIRECTORY SHADER_FOLDER_PATH)
    # message("SOURCE IS ${SHADER_FOLDER_PATH}\n")
    # message("DEST IS   ${CMAKE_CUSTOM_BUILD_OUTPUT_DIRECTORY}/shaders\n")
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${SHADER_FOLDER_PATH}
                ${CMAKE_CUSTOM_BUILD_OUTPUT_DIRECTORY}/shaders
    )
endfunction()




set(SOURCES
    source/scenetest/demo_render_fps_cube2.cpp
    source/scenetest/context.cpp
    source/scenetest/shader.cpp
    source/scenetest/sphere.cpp

    # source/basicexample.cpp
    # source/demo_singlethread_2d.cpp
    # source/demo_singlethread_2d_v2.cpp
    # source/hellocuda.cu


    # source/hellocuda.cu
    # include/hellocuda.hpp

    include/scenetest/context.hpp
    include/scenetest/shader.hpp
    include/scenetest/sphere.hpp

    include/colourmap_buf.hpp
)


add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE util2)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_shadercross::SDL3_shadercross)

# The minimum C/C++ versions required for the library
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_compile_features(${PROJECT_NAME} PUBLIC c_std_11)


if(MEASURE_PERFORMANCE_TIMEOUT)
    target_compile_definitions(${PROJECT_NAME} PRIVATE MEASURE_PERFORMANCE_TIMEOUT_FLAG=1)
    target_compile_definitions(${PROJECT_NAME} PRIVATE MEASURE_PERFORMANCE_TIMEOUT_ITERATIONS=25)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE MEASURE_PERFORMANCE_TIMEOUT_FLAG=0)
    target_compile_definitions(${PROJECT_NAME} PRIVATE MEASURE_PERFORMANCE_TIMEOUT_ITERATIONS=0)
endif()

set_target_properties(
    ${PROJECT_NAME} 
    PROPERTIES 
    CUDA_ARCHITECTURES native
)


compile_and_copy_shaders_to_binary_directory(${CMAKE_CURRENT_SOURCE_DIR}/source/scenetest/shaders)



add_custom_target(run_binary
    COMMAND ./${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_CUSTOM_BUILD_OUTPUT_DIRECTORY}
)


feature_summary(WHAT ENABLED_FEATURES
               INCLUDE_QUIET_PACKAGES
               DESCRIPTION "Enabled Features:"
               VAR enabledFeaturesText)
message(STATUS "${enabledFeaturesText}")