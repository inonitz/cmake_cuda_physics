#! /bin/bash


PROJECT_NAME=testing_cuda_with_cmake


mkdir -p build
cd build
cmake .. -G 'Ninja' -DCMAKE_C_COMPILER=clang-20 -DCMAKE_CXX_COMPILER=clang++-20 -DCMAKE_EXPORT_COMPILE_COMMANDS=1
ninja $PROJECT_NAME
ninja run
cd ..


# build_shared()
# {

# }


# build_static()
# {

# }


# clean_all()
# {
    
# }