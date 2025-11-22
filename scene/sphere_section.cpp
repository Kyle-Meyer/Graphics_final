#include "scene/sphere_section.hpp"

#include "geometry/geometry.hpp"

#include <cmath>

namespace cg
{

SphereSection::SphereSection() {}

SphereSection::SphereSection(float    min_lat,
                             float    max_lat,
                             uint32_t num_lat,
                             float    min_lon,
                             float    max_lon,
                             uint32_t num_lon,
                             float    radius,
                             int32_t  position_loc,
                             int32_t  normal_loc)
{
    // Convert to radians
    float min_lat_rad = degrees_to_radians(min_lat);
    float max_lat_rad = degrees_to_radians(max_lat);
    float min_lon_rad = degrees_to_radians(min_lon);
    float max_lon_rad = degrees_to_radians(max_lon);

    // Create a vertex list with unit length normals
    float           cos_lat, cos_lon, sin_lon;
    float           d_lat = (max_lat_rad - min_lat_rad) / static_cast<float>(num_lat);
    float           d_lon = (max_lon_rad - min_lon_rad) / static_cast<float>(num_lon);
    VertexAndNormal vtx;
    for(float curr_lon = min_lon_rad; curr_lon < max_lon_rad + EPSILON; curr_lon += d_lon)
    {
        cos_lon = std::cos(curr_lon);
        sin_lon = std::sin(curr_lon);
        for(float curr_lat = max_lat_rad; curr_lat >= min_lat_rad - EPSILON; curr_lat -= d_lat)
        {
            cos_lat = std::cos(curr_lat);
            vtx.normal.x = cos_lon * cos_lat;
            vtx.normal.y = sin_lon * cos_lat;
            vtx.normal.z = std::sin(curr_lat);
            vtx.vertex.x = radius * vtx.normal.x;
            vtx.vertex.y = radius * vtx.normal.y;
            vtx.vertex.z = radius * vtx.normal.z;
            vertices_.push_back(vtx);
        }
    }

    // Copy the first column of vertices
    for(uint32_t i = 0; i <= num_lat + 1; i++) { vertices_.push_back(vertices_[i]); }

    // Construct face list.  There are num_lat+1 rows and num_lon+1 columns. Create VBOs
    construct_row_col_face_list(num_lon + 1, num_lat + 1);
    create_vertex_buffers(position_loc, normal_loc);
}


SphereSection::SphereSection(float    min_lat,
                             float    max_lat,
                             uint32_t num_lat,
                             float    min_lon,
                             float    max_lon,
                             uint32_t num_lon,
                             float    radius,
                             int32_t  position_loc,
                             int32_t  normal_loc,
                             int32_t  texcoord_loc)
{
    // Convert to radians
    float min_lat_rad = degrees_to_radians(min_lat);
    float max_lat_rad = degrees_to_radians(max_lat);
    float min_lon_rad = degrees_to_radians(min_lon);
    float max_lon_rad = degrees_to_radians(max_lon);

    // Create a vertex list with unit length normals and texture coordinates
    float               cos_lat, cos_lon, sin_lon;
    float               d_lat = (max_lat_rad - min_lat_rad) / static_cast<float>(num_lat);
    float               d_lon = (max_lon_rad - min_lon_rad) / static_cast<float>(num_lon);
    VertexNormalTexture vtx;

    // Texture coordinate increments
    float du = 1.0f / static_cast<float>(num_lon);
    float dv = 1.0f / static_cast<float>(num_lat);
    float u = 0.0f;

    for(float curr_lon = min_lon_rad; curr_lon < max_lon_rad + EPSILON; curr_lon += d_lon)
    {
        cos_lon = std::cos(curr_lon);
        sin_lon = std::sin(curr_lon);

        float v = 0.0f; // Start from top of texture
        for(float curr_lat = max_lat_rad; curr_lat >= min_lat_rad - EPSILON; curr_lat -= d_lat)
        {
            cos_lat = std::cos(curr_lat);
            vtx.normal.x = cos_lon * cos_lat;
            vtx.normal.y = sin_lon * cos_lat;
            vtx.normal.z = std::sin(curr_lat);
            vtx.vertex.x = radius * vtx.normal.x;
            vtx.vertex.y = radius * vtx.normal.y;
            vtx.vertex.z = radius * vtx.normal.z;

            // Set texture coordinates
            vtx.texcoord.x = u;
            vtx.texcoord.y = v;

            vertices_with_tex_.push_back(vtx);
            v += dv;
        }
        u += du;
    }

    // Copy the first column of vertices with texture coordinate wrapping
    for(uint32_t i = 0; i <= num_lat + 1; i++)
    {
        VertexNormalTexture copy = vertices_with_tex_[i];
        copy.texcoord.x = 1.0f; // Wrap texture to close the seam
        vertices_with_tex_.push_back(copy);
    }

    // Construct face list.  There are num_lat+1 rows and num_lon+1 columns. Create VBOs
    construct_row_col_face_list(num_lon + 1, num_lat + 1);
    create_vertex_buffers(position_loc, normal_loc, texcoord_loc);
}

SphereSection::SphereSection(float    min_lat,
                             float    max_lat,
                             uint32_t num_lat,
                             float    min_lon,
                             float    max_lon,
                             uint32_t num_lon,
                             float    radius,
                             int32_t  position_loc,
                             int32_t  normal_loc,
                             int32_t  texcoord_loc,
                             int32_t  tangent_loc,
                             int32_t  bitangent_loc)
{
    // Convert to radians
    float min_lat_rad = degrees_to_radians(min_lat);
    float max_lat_rad = degrees_to_radians(max_lat);
    float min_lon_rad = degrees_to_radians(min_lon);
    float max_lon_rad = degrees_to_radians(max_lon);

    // Create vertex list with analytical tangent space
    float cos_lat, sin_lat, cos_lon, sin_lon;
    float d_lat = (max_lat_rad - min_lat_rad) / static_cast<float>(num_lat);
    float d_lon = (max_lon_rad - min_lon_rad) / static_cast<float>(num_lon);

    // Texture coordinate increments
    float du = 1.0f / static_cast<float>(num_lon);
    float dv = 1.0f / static_cast<float>(num_lat);
    float u = 0.0f;

    for(float curr_lon = min_lon_rad; curr_lon < max_lon_rad + EPSILON; curr_lon += d_lon)
    {
        cos_lon = std::cos(curr_lon);
        sin_lon = std::sin(curr_lon);

        float v = 0.0f;
        for(float curr_lat = max_lat_rad; curr_lat >= min_lat_rad - EPSILON; curr_lat -= d_lat)
        {
            cos_lat = std::cos(curr_lat);
            sin_lat = std::sin(curr_lat);

            VertexNormalTextureTangent vtx;

            // Normal points outward (same as position direction for unit sphere)
            vtx.normal.x = cos_lon * cos_lat;
            vtx.normal.y = sin_lon * cos_lat;
            vtx.normal.z = sin_lat;

            // Position
            vtx.vertex.x = radius * vtx.normal.x;
            vtx.vertex.y = radius * vtx.normal.y;
            vtx.vertex.z = radius * vtx.normal.z;

            // Texture coordinates
            vtx.texcoord.x = u;
            vtx.texcoord.y = v;

            // Analytical tangent: dP/d(lon) direction (eastward, around latitude circles)
            // For sphere: tangent = (-sin(lon), cos(lon), 0)
            vtx.tangent.x = -sin_lon;
            vtx.tangent.y = cos_lon;
            vtx.tangent.z = 0.0f;

            // Analytical bitangent: dP/d(lat) direction (northward, along meridians)
            // For sphere: bitangent = (-cos(lon)*sin(lat), -sin(lon)*sin(lat), cos(lat))
            vtx.bitangent.x = -cos_lon * sin_lat;
            vtx.bitangent.y = -sin_lon * sin_lat;
            vtx.bitangent.z = cos_lat;

            vertices_with_tangents_.push_back(vtx);
            v += dv;
        }
        u += du;
    }

    // Copy the first column with wrapped texture coordinates
    for(uint32_t i = 0; i <= num_lat + 1; i++)
    {
        VertexNormalTextureTangent copy = vertices_with_tangents_[i];
        copy.texcoord.x = 1.0f;
        vertices_with_tangents_.push_back(copy);
    }

    // Construct face list
    construct_row_col_face_list(num_lon + 1, num_lat + 1);

    // Create buffers directly (tangent space already computed analytically)
    has_tangent_space_ = true;
    create_vertex_buffers(position_loc, normal_loc, texcoord_loc, tangent_loc, bitangent_loc);
}

} // namespace cg
