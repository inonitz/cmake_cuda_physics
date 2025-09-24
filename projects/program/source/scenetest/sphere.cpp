#include "scenetest/sphere.hpp"
#include <cmath>


namespace u2m = util2::math; 


void generateSphere(
    i32 numLatitudeLines,   /* Lines South -> North */
    i32 numLongitudeLines,  /* Lines East  -> West  */
    i32 radius, 
    std::vector<u2m::vec3f>& outputVertices
) {
    /*
        Thanks to:
        https://gamedev.stackexchange.com/questions/150191/opengl-calculate-uv-sphere-vertices
        Also a pastebin if the link is dead:
        https://pastebin.com/276PwiYM
    */


    // One vertex at every latitude-longitude intersection,
    // plus one for the north pole and one for the south.
    // One meridian serves as a UV seam, so we double the vertices there.
    int numVertices = (numLatitudeLines * (numLongitudeLines + 1)) + 2;
    int numTriangles = numLatitudeLines * numLongitudeLines * 2;
    float latitudeSpacing = 1.0f / (numLatitudeLines + 1.0f);
    float longitudeSpacing = 1.0f / numLongitudeLines;
    int v = 1;

    std::vector<u2m::vec3f> positions(numVertices);
    std::vector<u2m::vec2f> texcoords(numVertices);
    std::vector<u2m::vec3f> vertices(numTriangles * 3);


    // North pole.
    positions[0] = u2m::vec3f(0, radius, 0);
    texcoords[0] = u2m::vec2f(0, 1);
    // South pole.
    positions[numVertices - 1] = u2m::vec3f(0, -radius, 0);
    texcoords[numVertices - 1] = u2m::vec2f(0, 0);


    for (int latitude = 0; latitude < numLatitudeLines; latitude++)
    {
        for (int longitude = 0; longitude <= numLongitudeLines; longitude++)
        {
            // Scale coordinates into the 0...1 texture coordinate range,
            // with north at the top (y = 1).
            texcoords[v] = u2m::vec2f(
                                longitude * longitudeSpacing,
                                1.0f - ((latitude + 1) * latitudeSpacing)
                            );
            // Convert to spherical coordinates:
            // theta is a longitude angle (around the equator) in radians.
            // phi is a latitude angle (north or south of the equator).
            float theta = (texcoords[v].x * 2.0f * u2m::pi<f32>);
            float phi   = ((texcoords[v].y - 0.5f) * u2m::pi<f32>);
            // This determines the radius of the ring of this line of latitude.
            // It's widest at the equator, and narrows as phi increases/decreases.
            float c = cosf(phi);
            // Usual formula for a vector in spherical coordinates.
            // You can exchange x & z to wind the opposite way around the sphere.
            positions[v] = u2m::vec3f(
                (c * cosf(theta)),
                sinf(phi),
                (c * sinf(theta))
            ) * __scast(f32, radius);

            v++;
        }
    }


    // Convert Vertices to triangles
    v = 0;
    for (int i = 0; i < numLongitudeLines; i++)
    {
        vertices[v++] = u2m::vec3f(positions[0]);
        vertices[v++] = u2m::vec3f(positions[i + 2]);
        vertices[v++] = u2m::vec3f(positions[i + 1]);
    }
    // Each row has one more unique vertex than there are lines of longitude,
    // since we double a vertex at the texture seam.
    int rowLength = numLongitudeLines + 1;
    for (int latitude = 0; latitude < numLatitudeLines - 1; latitude++)
    {
        // Plus one for the pole.
        int rowStart = (latitude * rowLength) + 1;
        for (int longitude = 0; longitude < numLongitudeLines; longitude++)
        {
            int firstCorner = rowStart + longitude;
            // First triangle of quad: Top-Left, Bottom-Left, Bottom-Right
            vertices[v++] = u2m::vec3f(positions[firstCorner]);
            vertices[v++] = u2m::vec3f(positions[firstCorner + rowLength + 1]);
            vertices[v++] = u2m::vec3f(positions[firstCorner + rowLength]);
            // Second triangle of quad: Top-Left, Bottom-Right, Top-Right
            vertices[v++] = u2m::vec3f(positions[firstCorner]);
            vertices[v++] = u2m::vec3f(positions[firstCorner + 1]);
            vertices[v++] = u2m::vec3f(positions[firstCorner + rowLength + 1]);
        }
    }


    u32 pole      = positions.size() - 1;
    u32 bottomRow = ((numLatitudeLines - 1) * rowLength) + 1;
    for (int i = 0; i < numLongitudeLines; i++)
    {
        vertices[v++] = u2m::vec3f(positions[pole]);
        vertices[v++] = u2m::vec3f(positions[bottomRow + i]);
        vertices[v++] = u2m::vec3f(positions[bottomRow + i + 1]);
    }


    outputVertices.swap(vertices);
    return;
}