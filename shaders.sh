#!/bin/bash


SHADER_SOURCE_DIRECTORY=projects/program/source/scenetest/shaders
VERTEX_SHADER_FILENAME=vert.glsl
FRAGMENT_SHADER_FILENAME=frag.glsl
COMPUTE_SHADER_FILENAME=comp.glsl


glslc -fshader-stage=vertex   "$SHADER_SOURCE_DIRECTORY/$VERTEX_SHADER_FILENAME"   -o "$SHADER_SOURCE_DIRECTORY/vert.spv"
glslc -fshader-stage=fragment "$SHADER_SOURCE_DIRECTORY/$FRAGMENT_SHADER_FILENAME" -o "$SHADER_SOURCE_DIRECTORY/frag.spv"
glslc -fshader-stage=compute  "$SHADER_SOURCE_DIRECTORY/$COMPUTE_SHADER_FILENAME"  -o "$SHADER_SOURCE_DIRECTORY/comp.spv"


# Consider moving this script to a custom cmake target