#version 460
layout (location = 0) in vec3 vertexPosition;
// layout (binding = 0)  in uniform sampler3D positionTex;


layout(std140, set = 0, binding = 3) uniform MatrixData {
    mat4x4 projection; 
}; 
layout(std140, set = 0, binding = 4) uniform MatrixData2 {
    mat4x4 view; 
}; 


ivec3 indexToTexelCoord3D(in ivec3 size, int index) {
    int zDirection = index % size.z;
    int yDirection = (index / size.z) % size.y;
    int xDirection = index / (size.y * size.z);
    return ivec3(xDirection, yDirection, zDirection);
}



// 1. uniform push consants for matrices
// 2. Need to add instancing with a 3D texutre
// 3. Uniform sampler3D at binding 0



void main()
{
    // gl_Position = vec4(a_position, 1.0f);
    // ivec3 texelPos = indexToTexelCoord3D(ivec3(ku_gridSize), gl_InstanceID);
    // vec4  pos      = texelFetch(positionTex, texelPos, 0);
    // gl_Position = projection * view * model * vec4(vertexPosition + pos.xyz, 1);
    gl_Position = projection * view * vec4(vertexPosition, 1);
    return;



}