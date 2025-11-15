#include "scene/surface_of_revolution.hpp"

#include <algorithm>

namespace cg
{

SurfaceOfRevolution::SurfaceOfRevolution() {}

SurfaceOfRevolution::SurfaceOfRevolution(std::vector<Point3> &v,
                                         uint32_t             n,
                                         int32_t              position_loc,
                                         int32_t              normal_loc)
{
    // Set number of rows and columns
    num_rows_ = static_cast<uint32_t>(v.size());
    num_cols_ = n + 1;

    // Add vertices to the vertex list, compute normals
    Vector3         normal, prev_normal;
    VertexAndNormal vtx;
    auto            vtx_iter_1 = v.begin();
    auto            vtx_iter_2 = vtx_iter_1 + 1;
    for(uint32_t i = 0; vtx_iter_2 != v.end(); vtx_iter_1++, vtx_iter_2++, i++)
    {
        normal = {vtx_iter_2->z - vtx_iter_1->z, 0.0f, vtx_iter_1->x - vtx_iter_2->x};
        normal.normalize();
        vtx.vertex = {vtx_iter_1->x, vtx_iter_1->y, vtx_iter_1->z};
        if(i == 0)
        {
            // Use normal for the first edge vertex
            vtx.normal = normal;
        }
        else
        {
            // Average normals of successive edges
            vtx.normal = (prev_normal + normal).normalize();
        }
        vertices_.push_back(vtx);

        // Copy normal for use in averaging
        prev_normal = normal;
    }

    // Store last vertex
    vtx.vertex = {vtx_iter_1->x, vtx_iter_1->y, vtx_iter_1->z};
    vtx.normal = normal;
    vertices_.push_back(vtx);

    // Reverse the vertex list so we go from top to bottom so
    // ConstructRowColFaceList forms ccw triangles
    std::reverse(vertices_.begin(), vertices_.end());

    // Create a rotation matrix
    Matrix4x4 m;
    m.rotate_z(360.0f / static_cast<float>(n));

    // Rotate the prior "edge" vertices
    uint32_t index = 0; // Index to the prior edge at this row
    for(uint32_t i = 0; i < n; i++)
    {
        for(uint32_t j = 0; j < num_rows_; j++)
        {
            // Rotate the vertex and the normal
            vtx.vertex = m * vertices_[index].vertex;
            vtx.normal = m * vertices_[index].normal;
            vertices_.push_back(vtx);
            index++;
        }
    }

    // Copy the first column of vertices
    for(uint32_t i = 0; i < num_rows_; i++) { vertices_.push_back(vertices_[i]); }

    // Construct the face list and create VBOs
    construct_row_col_face_list(num_cols_, num_rows_);
    create_vertex_buffers(position_loc, normal_loc);
}

SurfaceOfRevolution::SurfaceOfRevolution(std::vector<Point3> &v,
                                         uint32_t             n,
                                         int32_t              position_loc,
                                         int32_t              normal_loc,
                                         int32_t              texcoord_loc)
{
    // Set number of rows and columns
    num_rows_ = static_cast<uint32_t>(v.size());
    num_cols_ = n + 1;

    // Add vertices to the vertex list, compute normals and texture coordinates
    Vector3             normal, prev_normal;
    VertexNormalTexture vtx;
    auto                vtx_iter_1 = v.begin();
    auto                vtx_iter_2 = vtx_iter_1 + 1;
    
    // Calculate total height for v texture coordinate
    float min_z = v.front().z;
    float max_z = v.back().z;
    float height = max_z - min_z;
    
    for(uint32_t i = 0; vtx_iter_2 != v.end(); vtx_iter_1++, vtx_iter_2++, i++)
    {
        normal = {vtx_iter_2->z - vtx_iter_1->z, 0.0f, vtx_iter_1->x - vtx_iter_2->x};
        normal.normalize();
        vtx.vertex = {vtx_iter_1->x, vtx_iter_1->y, vtx_iter_1->z};
        
        if(i == 0)
        {
            // Use normal for the first edge vertex
            vtx.normal = normal;
        }
        else
        {
            // Average normals of successive edges
            vtx.normal = (prev_normal + normal).normalize();
        }
        
        // Set texture coordinates
        // u = 0 at the first column (will wrap around)
        // v is based on height along the profile
        // NOTE: Vertices will be reversed, so we invert v here so it's correct after reversal
        vtx.texcoord.x = 0.0f;
        vtx.texcoord.y = 1.0f - ((vtx_iter_1->z - min_z) / height);

        
        vertices_with_tex_.push_back(vtx);

        // Copy normal for use in averaging
        prev_normal = normal;
    }

    // Store last vertex
    vtx.vertex = {vtx_iter_1->x, vtx_iter_1->y, vtx_iter_1->z};
    vtx.normal = normal;
    vtx.texcoord.x = 0.0f;
    vtx.texcoord.y = 0.0f;  // Will be at bottom after reversal
    vertices_with_tex_.push_back(vtx);

    // Reverse the vertex list so we go from top to bottom so
    // ConstructRowColFaceList forms ccw triangles
    std::reverse(vertices_with_tex_.begin(), vertices_with_tex_.end());

    // Create a rotation matrix for each step
    Matrix4x4 rotation_step;
    rotation_step.rotate_z(360.0f / static_cast<float>(n));

    // Texture coordinate increment for each rotation step
    float du = 1.0f / static_cast<float>(n);

    // Rotate the prior "edge" vertices
    uint32_t index = 0; // Index to the prior edge at this row
    for(uint32_t i = 0; i < n; i++)
    {
        float u = static_cast<float>(i + 1) / static_cast<float>(n);
        
        for(uint32_t j = 0; j < num_rows_; j++)
        {
            // Rotate the vertex and the normal incrementally from previous column
            vtx.vertex = rotation_step * vertices_with_tex_[index].vertex;
            vtx.normal = rotation_step * vertices_with_tex_[index].normal;
            
            // Update u texture coordinate (wraps around the cylinder)
            vtx.texcoord.x = u;
            vtx.texcoord.y = vertices_with_tex_[index].texcoord.y;
            
            vertices_with_tex_.push_back(vtx);
            index++;
        }
    }

    // Copy the first column of vertices with texture coordinate wrapping
    for(uint32_t i = 0; i < num_rows_; i++)
    {
        VertexNormalTexture copy = vertices_with_tex_[i];
        copy.texcoord.x = 1.0f; // Wrap texture to close the seam
        vertices_with_tex_.push_back(copy);
    }

    // Construct the face list and create VBOs
    construct_row_col_face_list(num_cols_, num_rows_);
    create_vertex_buffers(position_loc, normal_loc, texcoord_loc);
}

} // namespace cg
