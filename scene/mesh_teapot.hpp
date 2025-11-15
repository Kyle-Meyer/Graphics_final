//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt, Brian Russin
//	File:    mesh_teapot.hpp
//	Purpose: Construction of the Utah teapot using recursive subdivision.
//============================================================================

#ifndef __SCENE_MESH_TEAPOT_HPP__
#define __SCENE_MESH_TEAPOT_HPP__

#include "geometry/point3.hpp"
#include "geometry/vector3.hpp"
#include "scene/tri_surface.hpp"

namespace cg
{

/**
 *
 */
class MeshTeapot : public TriSurface
{
  public:
    /**
     * Constructs the Utah teapot using uniform patch subdivision.  Reads in the
     * patch vertices and indices into a Vector3 array.  Recursively subdivides and store
     *	patches into a mesh surface.
     * @param level Number of levels to subdivide the patches
     *              Note: level cannot exceed 5, otherwise the number or vertices
     *                    exceeds the maximum allowed ( max<uint16_t> )
     */
    MeshTeapot(uint16_t level, int32_t position_loc, int32_t normal_loc);

  private:
    /**
     * Divides a patch represented by a 4x4 set of Vector3 vertices.  Each patch
     * is subdivided into 4 patches.  If the recursion level is 0, the patch is
     * stored in the mesh surface.
     * @param patch Patch (4x4 array of Vector3)
     * @param level Current level of subdivision
     */
    void divide_patch(std::array<std::array<Vector3, 4>, 4> patch, uint16_t level);

    /**
     * Helper function to divide a curve recursively until level == 0
     * @param ctrl_0 Beginning point of a curve
     * @param ctrl_1 Control point 1 of a curve
     * @param ctrl_2 Control point 2 of a curve
     * @param ctrl_3 End point of a curve
     * @param level Current level of subdivision
     * @return The fully sub-divided curve
     */
    static std::vector<Vector3> divide_curve(const Vector3 &ctrl_0,
                                             const Vector3 &ctrl_1,
                                             const Vector3 &ctrl_2,
                                             const Vector3 &ctrl_3,
                                             uint16_t       level);

    /**
     * Adds a vertex to the mesh; calls TriSurface::add_vertex if
     * 'find_existing' is true, otherwise appends 'v' to the back
     * of the mesh.
     * @param v Vector3 the vertex to add to the mesh
     * @param find_existing whether or not to search for an already
     *                      existing vertex at 'v'
     * @return The index of the added vertex in the mesh
     */
    uint16_t add_vertex(const Vector3 &v, bool find_existing);

    /**
     * Adds a sub-divided patch to the mesh
     * @param patch double vector of Vector3
     */
    void add_patch(const std::vector<std::vector<Vector3>> &patch);
};

} // namespace cg

#endif
