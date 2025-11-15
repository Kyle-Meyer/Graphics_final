#include "scene/unit_square.hpp"

#include "geometry/geometry.hpp"

#include <iostream>

namespace cg
{

UnitSquareSurface::UnitSquareSurface() {}

UnitSquareSurface::UnitSquareSurface(uint32_t n, int32_t position_loc, int32_t normal_loc)
{
    // Only allow 250 subdivision (so it creates less that 65K vertices)
    if(n > 250) n = 250;

    // Create VBOs and VAO
    // Normal is 0,0,1. z = 0 so all vertices lie in x,y plane.
    // Having issues with roundoff when n = 40,50 - so compare with some tolerance
    VertexAndNormal vtx;
    vtx.normal = {0.0f, 0.0f, 1.0f};
    vtx.vertex.z = 0.0f;
    float spacing = 1.0f / static_cast<float>(n);
    for(vtx.vertex.y = -0.5f; vtx.vertex.y <= 0.5f + EPSILON; vtx.vertex.y += spacing)
    {
        for(vtx.vertex.x = -0.5f; vtx.vertex.x <= 0.5f + EPSILON; vtx.vertex.x += spacing)
        {
            vertices_.push_back(vtx);
        }
    }

    // Construct the face list and create VBOs
    construct_row_col_face_list(n + 1, n + 1);
    create_vertex_buffers(position_loc, normal_loc);

    std::cout << "vertex list size = " << vertices_.size();
    std::cout << " face list size = " << faces_.size() << '\n';
}

UnitSquareSurface::UnitSquareSurface(uint32_t n, int32_t position_loc, int32_t normal_loc, int32_t tex_coord_loc)
{
    // Only allow 250 subdivision (so it creates less that 65K vertices)
    if(n > 250) n = 250;

    // Create VBOs and VAO with texture coordinates
    // Normal is 0,0,1. z = 0 so all vertices lie in x,y plane.
    // Texture coordinates map from (0,0) to (1,1) across the unit square
    VertexNormalTexture vtx;
    vtx.normal = {0.0f, 0.0f, 1.0f};
    vtx.vertex.z = 0.0f;
    
    float spacing = 1.0f / static_cast<float>(n);
    float tex_spacing = 1.0f / static_cast<float>(n);
    
    for(uint32_t row = 0; row <= n; ++row)
    {
        vtx.vertex.y = -0.5f + row * spacing;
        vtx.texcoord.y = static_cast<float>(row) * tex_spacing;
        
        for(uint32_t col = 0; col <= n; ++col)
        {
            vtx.vertex.x = -0.5f + col * spacing;
            vtx.texcoord.x = static_cast<float>(col) * tex_spacing;
            
            vertices_with_tex_.push_back(vtx);
        }
    }

    // Construct the face list and create VBOs with texture coordinates
    construct_row_col_face_list(n + 1, n + 1);
    create_vertex_buffers(position_loc, normal_loc, tex_coord_loc);

    std::cout << "vertex list size = " << vertices_with_tex_.size();
    std::cout << " face list size = " << faces_.size() << '\n';
}

UnitSquareSurface::UnitSquareSurface(uint32_t n, int32_t position_loc, int32_t normal_loc, int32_t tex_coord_loc, float tex_scale)
{
    // Only allow 250 subdivision (so it creates less that 65K vertices)
    if(n > 250) n = 250;

    // Create VBOs and VAO with texture coordinates and scaling
    // Normal is 0,0,1. z = 0 so all vertices lie in x,y plane.
    // Texture coordinates are scaled by tex_scale to allow tiling/repeating
    VertexNormalTexture vtx;
    vtx.normal = {0.0f, 0.0f, 1.0f};
    vtx.vertex.z = 0.0f;
    
    float spacing = 1.0f / static_cast<float>(n);
    float tex_spacing = tex_scale / static_cast<float>(n);  // Scale texture coordinates
    
    for(uint32_t row = 0; row <= n; ++row)
    {
        vtx.vertex.y = -0.5f + row * spacing;
        vtx.texcoord.y = static_cast<float>(row) * tex_spacing;
        
        for(uint32_t col = 0; col <= n; ++col)
        {
            vtx.vertex.x = -0.5f + col * spacing;
            vtx.texcoord.x = static_cast<float>(col) * tex_spacing;
            
            vertices_with_tex_.push_back(vtx);
        }
    }

    // Construct the face list and create VBOs with texture coordinates
    construct_row_col_face_list(n + 1, n + 1);
    create_vertex_buffers(position_loc, normal_loc, tex_coord_loc);

    std::cout << "vertex list size with texture = " << vertices_with_tex_.size();
    std::cout << " face list size = " << faces_.size() << '\n';
}

} // namespace cg
