//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:	 David W. Nesbitt
//	File:    surface_of_revolution.hpp
//	Purpose: Scene graph geometry node representing a surface or revolution.
//
//============================================================================

#ifndef __SCENE_SURFACE_OF_REVOLUTION_HPP__
#define __SCENE_SURFACE_OF_REVOLUTION_HPP__

#include "scene/tri_surface.hpp"

#include "geometry/point3.hpp"

namespace cg
{

class SurfaceOfRevolution : public TriSurface
{
  public:
    /**
     * Construct a surface of revolution given a list of points
     * where x and z are defined. x is the distance from the axis
     * and z is the height along the axis. Rotation around the z axis is
     * performed. Profile edge must be defined with increasing z.
     * @param  v   Edge/polyline to revolve
     * @param  n   Number of angular subdivisions
     */
    SurfaceOfRevolution(std::vector<Point3> &v,
                        uint32_t             n,
                        int32_t              position_loc,
                        int32_t              normal_loc);
    /**
     * Construct a surface of revolution with texture coordinates.
     * @param  v             Edge/polyline to revolve
     * @param  n             Number of angular subdivisions
     * @param  position_loc  Position attribute location
     * @param  normal_loc    Normal attribute location
     * @param  texcoord_loc  Texture coordinate attribute location
     */
    SurfaceOfRevolution(std::vector<Point3> &v,
                        uint32_t             n,
                        int32_t              position_loc,
                        int32_t              normal_loc,
                        int32_t              texcoord_loc);
  private:
    uint32_t num_rows_;
    uint32_t num_cols_;

    // Make default constructor private to force use of the constructor
    // with number of subdivisions.
    SurfaceOfRevolution();
};

} // namespace cg

#endif
