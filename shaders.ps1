$SHADER_SOURCE_DIRECTORY      = 'projects/program/source/scenetest/shaders/source'
$SHADER_BUILD_OUTPUT_DIRECTORY= 'projects/program/source/scenetest/shaders/bin'
$VERTEX_SHADER_FILENAME   = 'vert.glsl'
$FRAGMENT_SHADER_FILENAME = 'frag.glsl'
$COMPUTE_SHADER_FILENAME  = 'comp.glsl'


glslc -fshader-stage=vertex   "$SHADER_SOURCE_DIRECTORY/$VERTEX_SHADER_FILENAME"   -o "$SHADER_BUILD_OUTPUT_DIRECTORY/vert.spv"
glslc -fshader-stage=fragment "$SHADER_SOURCE_DIRECTORY/$FRAGMENT_SHADER_FILENAME" -o "$SHADER_BUILD_OUTPUT_DIRECTORY/frag.spv"
glslc -fshader-stage=compute  "$SHADER_SOURCE_DIRECTORY/$COMPUTE_SHADER_FILENAME"  -o "$SHADER_BUILD_OUTPUT_DIRECTORY/comp.spv"


# Consider moving this script to a custom cmake target