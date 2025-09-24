#ifndef __SPHERE_VERTICES_GENERATOR_HEADER__
#define __SPHERE_VERTICES_GENERATOR_HEADER__
#include <vector>
#include <util2/C/base_type.h>
#include <util2/vec2.hpp>


void generateSphere(
    i32 numLatitudeLines,   /* Lines South -> North */
    i32 numLongitudeLines,  /* Lines East  -> West  */
    i32 radius, 
    std::vector<util2::math::vec3f>& outputVertices
);


#endif /* __SPHERE_VERTICES_GENERATOR_HEADER__ */
