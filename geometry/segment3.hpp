//============================================================================
//	Johns Hopkins University Engineering for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt
//	File:    segment3.hpp
//	Purpose: 3D Line segment class.
//
//============================================================================

#ifndef __GEOMETRY_SEGMENT3_HPP__
#define __GEOMETRY_SEGMENT3_HPP__

#include "geometry/point3.hpp"

#include <vector>

namespace cg
{

// Forward Declarations
struct Segment3PointDistanceResult;

/**
 * Line segment in 3-D. Data members are the 2 endpoints of the segment: a and b.
 */
struct LineSegment3
{
    Point3 a;
    Point3 b;

    /**
     * Constructor given 2 points.
     * @param   p1    First point of the segment.
     * @param   p2    Second point of the segment.
     */
    LineSegment3(const Point3 &p1, const Point3 &p2);

    /**
     * Finds the distance of a specified point from the line segment
     * and the closest point on the segement to the specified point.
     * @param   p         Test point.
     * @return Returns the distance from p to the closest point on
     *         the segment, and the closest point on the segment to point p.
     */
    Segment3PointDistanceResult distance(const Point3 &p) const;
};

struct Segment3PointDistanceResult
{
    float  distance;
    Point3 closest_point;
};

} // namespace cg

#endif
