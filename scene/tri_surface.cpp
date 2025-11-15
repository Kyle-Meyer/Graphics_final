#include "scene/tri_surface.hpp"

namespace cg
{

TriSurface::TriSurface() : vao_{0}, vbo_{0}, facebuffer_{0}, has_texture_coords_{false}, GeometryNode()
{
}

TriSurface::~TriSurface()
{
    // Delete vertex buffer objects
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &facebuffer_);
    glDeleteVertexArrays(1, &vao_);
}

void TriSurface::draw(SceneState &scene_state)
{
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, face_count_, GL_UNSIGNED_SHORT, (void *)0);
    glBindVertexArray(0);
}

void TriSurface::construct(const std::vector<VertexAndNormal> &v, const std::vector<uint16_t> &f)
{
    vertices_ = v;
    faces_ = f;
    has_texture_coords_ = false;
}

void TriSurface::add_polygon(const std::vector<Point3> &vertex_list)
{
    // Form the normal
    VertexAndNormal vertex;
    Vector3         e1(vertex_list[0], vertex_list[1]);
    Vector3         e2(vertex_list[0], vertex_list[2]);
    vertex.normal = (e1.cross(e2)).normalize();

    // Add vertices to the vertex list (with the face normal).
    // Save the current index so the face list is properly constructed
    size_t curr_vertex = vertices_.size();
    for(const auto &v : vertex_list)
    {
        vertex.vertex = v;
        vertices_.push_back(vertex);
    }

    // Form face list as if this was a triangle fan (always use the 1st vertex)
    for(size_t i = 2, n = vertex_list.size(); i < n; i++)
    {
        faces_.push_back(curr_vertex);
        faces_.push_back(curr_vertex + i - 1);
        faces_.push_back(curr_vertex + i);
    }
}

void TriSurface::add(const Point3 &v0, const Point3 &v1, const Point3 &v2)
{
    faces_.push_back(add_vertex(v0));
    faces_.push_back(add_vertex(v1));
    faces_.push_back(add_vertex(v2));
}

void TriSurface::end(int32_t position_loc, int32_t normal_loc)
{
    // Iterate through the face list and calculate the normals for each
    // face and add the normal to each vertex in the face list. This
    // assumes the vertex normals are initilaized to 0 (in constructor
    // of VertexAndNormal)
    uint32_t v0, v1, v2;
    Vector3  e1, e2, face_normal;
    auto     face_vertex = faces_.begin();
    while(face_vertex != faces_.end())
    {
        // Get the vertices of the face (assumes ccw order)
        v0 = *face_vertex++;
        v1 = *face_vertex++;
        v2 = *face_vertex++;

        // Calculate surface normal. Normalize it since cross products
        // do not ensure unit length normals. We need to make sure normals
        // are same length so averaging works properly
        e1.set(vertices_[v0].vertex, vertices_[v1].vertex);
        e2.set(vertices_[v0].vertex, vertices_[v2].vertex);
        face_normal = (e1.cross(e2)).normalize();

        // Add the face normal to the vertex normals of the triangle
        vertices_[v0].normal += face_normal;
        vertices_[v1].normal += face_normal;
        vertices_[v2].normal += face_normal;
    }

    // Normalize the vertex normals - this essentially averages the
    // adjoining face normals.
    for(auto &v : vertices_) { v.normal.normalize(); }

    // Create the vertex and face buffers
    create_vertex_buffers(position_loc, normal_loc);
}

void TriSurface::create_vertex_buffers(int32_t position_loc, int32_t normal_loc)
{
    has_texture_coords_ = false;

    // Generate vertex buffers for the vertex list and the face list
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &facebuffer_);

    // Bind the vertex list to the vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices_.size() * sizeof(VertexAndNormal),
                 (void *)&vertices_[0],
                 GL_STATIC_DRAW);

    // Bind the face list to the vertex buffer object
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facebuffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 faces_.size() * sizeof(uint16_t),
                 (void *)&faces_[0],
                 GL_STATIC_DRAW);

    // Copy the face list count for use in Draw
    face_count_ = static_cast<GLsizei>(faces_.size());

    // Allocate a VAO, enable it and set the vertex attribute arrays and pointers
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    // Bind the vertex buffer, set the vertex position attribute and the vertex normal attribute
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAndNormal), (void *)0);
    glVertexAttribPointer(
        normal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAndNormal), (void *)(sizeof(Point3)));
    glEnableVertexAttribArray(position_loc);
    glEnableVertexAttribArray(normal_loc);

    // Bind the face list buffer and draw.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facebuffer_);

    // Make sure changes to this VAO are local
    glBindVertexArray(0);
}

// Version with texture coordinates
void TriSurface::create_vertex_buffers(int32_t position_loc, int32_t normal_loc, int32_t texcoord_loc)
{
    has_texture_coords_ = true;

    // Generate vertex buffers for the vertex list and the face list
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &facebuffer_);

    // Bind the vertex list to the vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices_with_tex_.size() * sizeof(VertexNormalTexture),
                 (void *)&vertices_with_tex_[0],
                 GL_STATIC_DRAW);

    // Bind the face list to the vertex buffer object
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facebuffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 faces_.size() * sizeof(uint16_t),
                 (void *)&faces_[0],
                 GL_STATIC_DRAW);

    // Copy the face list count for use in Draw
    face_count_ = static_cast<GLsizei>(faces_.size());

    // Allocate a VAO, enable it and set the vertex attribute arrays and pointers
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    // Bind the vertex buffer, set the vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    
    // Position attribute (location 0)
    if(position_loc >= 0)
    {
        glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, 
                             sizeof(VertexNormalTexture), 
                             (void *)0);
        glEnableVertexAttribArray(position_loc);
    }
    
    // Normal attribute (location 1)
    if(normal_loc >= 0)
    {
        glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, 
                             sizeof(VertexNormalTexture), 
                             (void *)(sizeof(Point3)));
        glEnableVertexAttribArray(normal_loc);
    }
    
    //Texture coordinate attribute (location 2)
    if(texcoord_loc >= 0)
    {
        glVertexAttribPointer(texcoord_loc, 2, GL_FLOAT, GL_FALSE, 
                             sizeof(VertexNormalTexture), 
                             (void *)(sizeof(Point3) + sizeof(Vector3)));
        glEnableVertexAttribArray(texcoord_loc);
    }

    // Bind the face list buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facebuffer_);

    // Make sure changes to this VAO are local
    glBindVertexArray(0);
}

void TriSurface::construct_row_col_face_list(uint32_t num_rows, uint32_t num_cols)
{
    for(uint32_t row = 0; row < num_rows - 1; row++)
    {
        for(uint32_t col = 0; col < num_cols - 1; col++)
        {
            // Divide each square into 2 triangles - make sure they are ccw.
            // GL_TRIANGLES draws independent triangles for each set of 3 vertices
            faces_.push_back(get_index(row + 1, col, num_cols));
            faces_.push_back(get_index(row, col, num_cols));
            faces_.push_back(get_index(row, col + 1, num_cols));

            faces_.push_back(get_index(row + 1, col, num_cols));
            faces_.push_back(get_index(row, col + 1, num_cols));
            faces_.push_back(get_index(row + 1, col + 1, num_cols));
        }
    }
}

uint16_t TriSurface::get_index(uint32_t row, uint32_t col, uint32_t num_cols) const
{
    return static_cast<uint16_t>((row * num_cols) + col);
}

uint16_t TriSurface::add_vertex(const Point3 &vtx)
{
    // Check if vertex is in the list. This is just a brute force method.
    // Efficiency can be improved but we only use this at startup
    uint16_t index = 0;
    for(const auto &v : vertices_)
    {
        if(vtx == v.vertex) { return index; }
        index++;
    }

    // Not in the list, add it. Make sure the vertex normal is initialized
    // to (0,0,0)
    VertexAndNormal vertex_and_normal(vtx);
    vertices_.push_back(vertex_and_normal);
    return static_cast<uint16_t>(vertices_.size() - 1);
}

} // namespace cg
