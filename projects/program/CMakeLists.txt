include(CMakePrintHelpers)
include(FeatureSummary)


set(SOURCES
    source/hello.cpp
    source/main.cpp
    source/hellocuda.cu

    include/hello.hpp
    include/hellocuda.hpp

)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_PROJECT_DIRECTORY}/program/include")

set_property(TARGET ${PROJECT_NAME} PROPERTY CUDA_ARCHITECTURES native)


add_custom_target(
    run ALL
    COMMAND ./${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
)


feature_summary(WHAT ENABLED_FEATURES
               INCLUDE_QUIET_PACKAGES
               DESCRIPTION "Enabled Features:"
               VAR enabledFeaturesText)
message(STATUS "${enabledFeaturesText}")
