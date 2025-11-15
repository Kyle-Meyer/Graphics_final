#include "scene/conic.hpp"

#include "geometry/geometry.hpp"

#include <cmath>

namespace cg
{

ConicSurface::ConicSurface() {}

ConicSurface::ConicSurface(float    bottom_radius,
                           float    top_radius,
                           uint32_t num_sides,
                           uint32_t num_stacks,
                           int32_t  position_loc,
                           int32_t  normal_loc)
{
    // Fail if top and bottom radius are both 0
    if(bottom_radius <= 0.0f && top_radius <= 0.0f) return;

    // There are num_stacks+1 rows in the vertex list and num_sides columns
    num_rows_ = num_stacks + 1;
    num_cols_ = num_sides + 1;

    // Set a rotation matrix for the normals
    Matrix4x4 m;
    m.rotate_z(360.0f / static_cast<float>(num_sides));

    // Create a normal at theta = 0 perpendicular to vector along side. Note
    // that if we use a 2D vector in the x,z plane to represent the side
    // vector then we just swap vertices and negate to find a perpendicular
    Vector3 n(1.0f, 0.0f, (bottom_radius - top_radius));
    n.normalize();

    // Create a vertex list. We change radius linearly from the top
    // radius to bottom radius
    uint32_t        i, j;
    float           z, r;
    float           dz = 1.0f / static_cast<float>(num_stacks);
    float           dr = (bottom_radius - top_radius) / static_cast<float>(num_stacks);
    float           theta = 0.0f;
    float           d_theta = (2.0f * PI) / static_cast<float>(num_sides);
    float           cos_theta, sin_theta;
    VertexAndNormal vtx;

    for(i = 0; i < num_sides; i++, theta += d_theta)
    {
        // Compute sin,cos to use in the loop below
        cos_theta = std::cos(theta);
        sin_theta = std::sin(theta);

        // Set the normal for this side
        vtx.normal = n;

        // Form vertices along the side for this angle increment
        // Iterate from top to bottom so we create ccw triangles
        for(j = 0, z = 0.5f, r = top_radius; j <= num_stacks; j++, z -= dz, r += dr)
        {
            vtx.vertex.set(r * cos_theta, r * sin_theta, z);
            vertices_.push_back(vtx);
        }

        // Rotate the normal
        n = m * n;
    }

    // Copy the first column of vertices
    for(uint32_t i = 0; i < num_rows_; i++) { vertices_.push_back(vertices_[i]); }

    // Construct the face list and create VBOs
    construct_row_col_face_list(num_cols_, num_rows_);
    create_vertex_buffers(position_loc, normal_loc);
}

} // namespace cg
