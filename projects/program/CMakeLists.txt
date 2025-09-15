include(CMakePrintHelpers)
include(FeatureSummary)


set(SOURCES
    source/main.cpp
    source/hellocuda.cu

    include/colourmap_buf.hpp
    include/hellocuda.hpp
)


add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE util2)
# target_include_directories()


set_target_properties(
    ${PROJECT_NAME} 
    PROPERTIES 
    CUDA_ARCHITECTURES native
)


add_custom_target(
    run ALL
    COMMAND ./${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
)

add_custom_target(run_binary
    COMMAND ./${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../../$<CONFIG>"
)


feature_summary(WHAT ENABLED_FEATURES
               INCLUDE_QUIET_PACKAGES
               DESCRIPTION "Enabled Features:"
               VAR enabledFeaturesText)
message(STATUS "${enabledFeaturesText}")