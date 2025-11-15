#include "scene/torus.hpp"

#include "geometry/geometry.hpp"

#include <cmath>

namespace cg
{

TorusSurface::TorusSurface() {}

TorusSurface::TorusSurface(float    ring_radius,
                           float    tube_radius,
                           uint32_t num_ring,
                           uint32_t num_tube,
                           int32_t  position_loc,
                           int32_t  normal_loc)
{
    // Use <= so we wrap around to make the last vertices meet the first
    int32_t         i, j;
    float           v, phi, theta;
    float           d_phi = (2.0f * PI) / static_cast<float>(num_tube);
    float           d_theta = (2.0f * PI) / static_cast<float>(num_ring);
    VertexAndNormal vtx;
    for(phi = 0.0f, j = 0; j <= num_tube; j++, phi += d_phi)
    {
        for(theta = 0.0f, i = 0; i <= num_ring; i++, theta += d_theta)
        {
            // Compute vertex
            v = (ring_radius + tube_radius * std::cos(phi));
            vtx.vertex.set(v * std::cos(theta), v * std::sin(theta), tube_radius * std::sin(phi));

            // Compute normal. It is the cross product of the two tangents (one
            // with respect to the ring rotation and one with repect to the tube
            // rotation). These are found by taking the derivative of the
            // parametric equation with respect to theta and phi.
            Vector3 tan1(-std::sin(theta), std::cos(theta), 0.0f);
            Vector3 tan2(std::cos(theta) * (-std::sin(phi)),
                         std::sin(theta) * (-std::sin(phi)),
                         std::cos(phi));
            vtx.normal = (tan1.cross(tan2)).normalize();
            vertices_.push_back(vtx);
        }
    }

    // Construct face list and create VBOs. There are num_tube+1 rows (outer for
    // loop) and num_ring+1 columns (inner for loop).
    construct_row_col_face_list(num_tube + 1, num_ring + 1);
    create_vertex_buffers(position_loc, normal_loc);
}

} // namespace cg
