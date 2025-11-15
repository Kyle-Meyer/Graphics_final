//============================================================================
//	Johns Hopkins University Engineering for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt
//	File:    segment2.hpp
//	Purpose: 2D Line segment class. Includes clipping code.
//============================================================================

#ifndef __GEOMETRY_SEGMENT2_HPP__
#define __GEOMETRY_SEGMENT2_HPP__

#include "geometry/point2.hpp"

#include <vector>

namespace cg
{

// Forward Declarations
struct Segment2PointDistanceResult;
struct Segment2IntersectionResult;
struct Segment2ClipResult;

struct CRectangle
{
    float left;
    float right;
    float bottom;
    float top;
};

/**
 * Line segment in 2D. Data members are the 2 endpoints of the segment: a and b.
 */
struct LineSegment2
{
    Point2 a;
    Point2 b;

    /**
     * Default constructor.
     */
    LineSegment2();

    /**
     * Constructor given 2 points.
     * @param   p1    First point of the segment.
     * @param   p2    Second point of the segment.
     */
    LineSegment2(const Point2 &p1, const Point2 &p2);

    /**
     * Finds the distance of a specified point from the line segment
     * and the closest point on the segement to the specified point.
     * @param  p Test point.
     * @return Returns the distance from p to the closest point on
     *         the segment, and the closest point on the segment to point p.
     */
    Segment2PointDistanceResult distance(const Point2 &p) const;

    /**
     * Determines if the current segment intersects the specified segment.
     * If an intersect occurs the intersect_pt is determined.  Note: the
     * case where the lines overlap is not considered. Consider any parallel
     * line segment case to be no intersect (return false).
     * @param  segment        Segment to determine intersection with.
     * @return Returns true if an intersection exists (false if not),
     *         and the intersection point;
     */
    Segment2IntersectionResult intersect(const LineSegment2 &segment) const;

    /**
     * Clips the line segment to a specified convex polygon.
     * @param  poly A counter-clockwise oriented polygon.
     * @return Returns true if any part of the segment remains and
     *         the clipped segment.
     */
    Segment2ClipResult clip_to_polygon(const std::vector<Point2> &poly) const;

    /**
     * Clips the line segment to a specified rectangle.
     * @param  r  A rectangle
     * @return Returns true if any part of the segment remains and
     *         the clipped segment.
     */
    Segment2ClipResult clip_to_rectangle(const CRectangle &r) const;
};

struct Segment2PointDistanceResult
{
    float  distance;
    Point2 closest_point;
};

struct Segment2IntersectionResult
{
    bool   intersects;
    Point2 intersect_point;
};

struct Segment2ClipResult
{
    bool         clipped;
    LineSegment2 clip_segment;
};

} // namespace cg

#endif
