#!/bin/bash


PROJECT_NAME=testing_cuda_with_cmake
CMAKE_ARGLIST="\
    -DCMAKE_C_COMPILER=clang-20 \
    -DCMAKE_CXX_COMPILER=clang++-20 \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
    -DGIT_SUBMODULE=1 \
    -DBUILD_SHARED_LIBS=1 \
    -DCMAKE_COLOR_DIAGNOSTICS=ON \
"
CMAKE_ROOT_BUILD_DIR="build"
CMAKE_BUILD_DIR=""
CLEAN_ROOT_BUILD_DIR=true





# argument parsing
if [ $1 = "debug" ];
then
    CMAKE_BUILD_DIR+="debug"
    CMAKE_ARGLIST+=" -DCMAKE_BUILD_TYPE=Debug"
elif [ $1 = "release" ];
then
    CMAKE_BUILD_DIR+="release"
    CMAKE_ARGLIST+=" -DCMAKE_BUILD_TYPE=Release"
elif [ $1 = "release_dbginfo" ];
then
    CMAKE_BUILD_DIR+="relWithDbgInfo"
    CMAKE_ARGLIST+=" -DCMAKE_BUILD_TYPE=RelWithDbgInfo"
else
    printf "Unknown Argument %s - valid values are: debug, release, release_dbginfo\nExiting..." $1
    exit
fi


if [ $2 = "shared" ];
then
    CMAKE_BUILD_DIR+="/shared"
    CMAKE_ARGLIST+=" -DBUILD_SHARED_LIBS=1"
elif [ $2 = "static" ];
then
    CMAKE_BUILD_DIR+="/static"
    CMAKE_ARGLIST+=" -DBUILD_SHARED_LIBS=0"
else
    printf "Unknown Argument %s - valid values are: shared, static\nExiting..." $2
    exit
fi


if [ $3 = "clean" ];
then
    CLEAN_ROOT_BUILD_DIR=true
elif [ $3 = "noclean" ];
then
    CLEAN_ROOT_BUILD_DIR=false
else
    printf "Unknown Argument %s - valid values are: clean, noclean\nExiting..." $3
    exit
fi




# the actual script
CMAKE_FINAL_BUILD_DIR="$CMAKE_ROOT_BUILD_DIR/$CMAKE_BUILD_DIR"
echo $CMAKE_FINAL_BUILD_DIR
echo $CMAKE_ARGLIST


mkdir -p build
if [ "$CLEAN_ROOT_BUILD_DIR" = "true" ];
then
    rm -rf $CMAKE_FINAL_BUILD_DIR
fi
mkdir -p $CMAKE_FINAL_BUILD_DIR
cd $CMAKE_FINAL_BUILD_DIR
cmake ../../../ -G 'Ninja' $CMAKE_ARGLIST
cp "compile_commands.json" "../../compile_commands.json" 
ninja $PROJECT_NAME
ninja run_binary
cd ..
