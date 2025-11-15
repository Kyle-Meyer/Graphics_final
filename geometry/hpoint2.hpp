//============================================================================
//	Johns Hopkins University Engineering for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt
//	File:    hpoint2.hpp
//	Purpose: Structure to hold 2D homogenous coordinates
//
//============================================================================

#ifndef __GEOMETRY_HPOINT2_HPP__
#define __GEOMETRY_HPOINT2_HPP__

#include "geometry/point2.hpp"

namespace cg
{

/**
 * 2D Homogeneous coordinate
 */
struct HPoint2
{
    float x;
    float y;
    float w;

    /**
     * Default constructor
     */
    HPoint2();

    /**
     * Constructor with initial values for x,y,w.
     * @param   ix   x coordinate position.
     * @param   iy   y coordinate position.
     * @param   iw   Homogeneous factor
     */
    HPoint2(float ix, float iy, float iw);

    /**
     * Convert to a cartesian representation.
     * @return  Returns the cartesian representation of this point.
     */
    Point2 to_cartesian() const;
};

} // namespace cg

#endif
