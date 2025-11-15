//============================================================================
//	Johns Hopkins University Engineering for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt
//	File:    hpoint3.hpp
//	Purpose: Structures to hold 3D homogenous coordinates
//
//============================================================================

#ifndef __GEOMETRY_HPOINT3_HPP__
#define __GEOMETRY_HPOINT3_HPP__

#include "geometry/point3.hpp"

namespace cg
{

/**
 * Homogeneous coordinate
 */
struct HPoint3
{
    float x;
    float y;
    float z;
    float w;

    /**
     * Default constructor
     */
    HPoint3();

    /**
     * Constructor with initial values for x,y,z,w.
     * @param   ix   x coordinate position.
     * @param   iy   y coordinate position.
     * @param   iz   z coordinate position.
     * @param   iw   Homogeneous factor
     */
    HPoint3(float ix, float iy, float iz, float iw);

    /**
     * Convert to a cartesian representation
     * @return  Returns the cartesian representation of this point.
     */
    Point3 to_cartesian() const;
};

} // namespace cg

#endif
