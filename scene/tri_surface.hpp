//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:	  David W. Nesbitt
//	File:     tri_surface.hpp
//	Purpose:  Scene graph geometry node indicating a triangle mesh based
//            surface. Uses indexed vertex arrays and per vertex normals.
//
//============================================================================

#ifndef __SCENE_TRI_SURFACE_HPP__
#define __SCENE_TRI_SURFACE_HPP__

#include "scene/geometry_node.hpp"

namespace cg
{

/**
 * Triangle mesh surface. Uses indexed vertex arrays. Stores
 * vertices as VertexAndNormal or VertexNormalTexture.
 */
class TriSurface : public GeometryNode
{
  public:
    /**
     * Constructor.
     */
    TriSurface();

    /**
     * Destructor.
     */
    ~TriSurface();

    /**
     * Draw this geometry node.
     */
    void draw(SceneState &scene_state) override;

    /**
     * Construct triangle surface by passing in vertex list and face list
     * @param  v  List of vertices (position and normal)
     * @param  f    Index list for triangles
     */
    void construct(const std::vector<VertexAndNormal> &v, const std::vector<uint16_t> &f);

    /**
     * Adds the vertices of the triangle to the vertex list. Accounts for
     * shared vertices by checking if the vertex is already in the list.
     * Manages the face list as well - stores indexes into the vertex list
     * for this triangle. This method allows triangles to be added one at
     * a time to a curved surface. Call End() when done to get vertex normal
     * assigned. Vertices should be oriented counter-clockwise.
     * @param  v0  First vertex in the triangle
     * @param  v1  Second vertex in the triangle
     * @param  v2  Last vertex in the triangle
     */
    void add(const Point3 &v0, const Point3 &v1, const Point3 &v2);

    /**
     * Adds the vertices of a convex polygon to the vertex list. Forms faces
     * as if a triangle fan was constructed.
     * @param  vertex_list  List of vertices in the triangle (ccw orientation)
     */
    void add_polygon(const std::vector<Point3> &vertex_list);

    /**
     * Marks the end of a triangle mesh. Calculates the vertex normals.
     */
    void end(int32_t position_loc, int32_t normal_loc);

    /**
     * Creates vertex buffers for this object (without texture coordinates).
     */
    void create_vertex_buffers(int32_t position_loc, int32_t normal_loc);

    /**
     * Creates vertex buffers for this object (with texture coordinates).
     */
    void create_vertex_buffers(int32_t position_loc, int32_t normal_loc, int32_t texcoord_loc);

  protected:
    // Vertex buffer support
    GLsizei face_count_;
    GLuint  vao_;
    GLuint  vbo_;
    GLuint  facebuffer_;

    // Vertex and normal list
    std::vector<VertexAndNormal> vertices_;

    // Vertex, normal, and texture coordinate list
    std::vector<VertexNormalTexture> vertices_with_tex_;
    bool                             has_texture_coords_;

    // Use uint16_t for face list indexes (OpenGL ES compatible)
    std::vector<uint16_t> faces_;

    /**
     * Form triangle face indexes for a surface constructed using a double loop -
     * one can be considered rows of the surface and the other can be considered
     * columns of the surface. Assumes the vertex list is populated in "row"
     * order.
     * @param  num_rows  Number of rows
     * @param  num_cols  Number of columns
     */
    void construct_row_col_face_list(uint32_t num_rows, uint32_t num_cols);

    // Convenience method to get the index into the vertex list given the
    // "row" and "column" of the subdivision/grid
    uint16_t get_index(uint32_t row, uint32_t col, uint32_t num_cols) const;

    /**
     * Adds a vertex to the surface vertex list.  Returns the index into the
     * vertex list.  If the vertex is already in the list it does not
     * replicate it.
     * @param  vtx  Vertex
     */
    uint16_t add_vertex(const Point3 &vtx);
};

} // namespace cg

#endif
