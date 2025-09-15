set(SOURCES
    source/aligned_malloc.cpp
    source/C/aligned_malloc.c
    source/C/buffer.c
    source/C/file.c
    source/C/ifcrash2.c
    source/C/image2.c
    source/C/marker4.c
    source/C/print.c
    source/C/stb_image.c
    source/C/thread_sleep.c
    source/file.cpp
    source/ifcrash.cpp
    source/pool.cpp
    source/print.cpp
    source/random.cpp
    source/time.cpp
    source/vec2.cpp

    include/util2/aligned_malloc.hpp
    include/util2/array_view.hpp
    include/util2/bufferptr.hpp
    include/util2/C/aligned_malloc.h
    include/util2/C/base_type.h
    include/util2/C/buffer.h
    include/util2/C/compiler_warning.h
    include/util2/C/file.h
    include/util2/C/ifcrash2.h
    include/util2/C/image2.h
    include/util2/C/macro.h
    include/util2/C/marker4.h
    include/util2/C/murmur64a.h
    include/util2/C/print.h
    include/util2/C/static_assert.h
    include/util2/C/stb_image.h
    include/util2/C/string.h
    include/util2/C/thread_sleep.h
    include/util2/C/util2_api.h
    include/util2/C/util2_extern.h
    include/util2/C/varg_count.h
    include/util2/file.hpp
    include/util2/flat_hash.hpp
    include/util2/hash.hpp
    include/util2/ifcrash.hpp
    include/util2/pool.hpp
    include/util2/print.hpp
    include/util2/random.hpp
    include/util2/string.hpp
    include/util2/time.hpp
    include/util2/type_trait.hpp
    include/util2/vec2.hpp

)


# Didn't define debug/release/etc...
if(NOT DEFINED CMAKE_BUILD_TYPE)
    # set(cmake_build_type_undefined 1)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "CMake build type" FORCE)
    endif()

# The minimum C/C++ versions required for the library
if(NOT DEFINED CMAKE_C_STANDARD)
    set(CMAKE_C_STANDARD 11)
endif()
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()


if(BUILD_SHARED_LIBS)
    add_library(util2 SHARED ${SOURCES})
    target_compile_definitions(util2 PUBLIC UTIL2_EXPORTS)
else()
    add_library(util2 STATIC ${SOURCES})
    target_compile_definitions(util2 PUBLIC UTIL2_STATIC_DEFINE)
endif()


target_include_directories(util2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(util2 PRIVATE pthread)


feature_summary(WHAT ENABLED_FEATURES
               INCLUDE_QUIET_PACKAGES
               DESCRIPTION "Enabled Features:"
               VAR enabledFeaturesText)
message(STATUS "${enabledFeaturesText}")