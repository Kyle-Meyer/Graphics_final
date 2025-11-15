//============================================================================
//	Johns Hopkins University Engineering for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt
//	File:    point2.hpp
//	Purpose: Structure to hold 2D coordinates
//
//============================================================================

#ifndef __GEOMETRY_POINT2_HPP__
#define __GEOMETRY_POINT2_HPP__

#include <vector>

namespace cg
{

// Forward Declarations
struct HPoint2;
struct Vector2;

/**
 * 2D Point (cartesian)
 */
struct Point2
{
    float x;
    float y;

    /**
     * Default constructor
     */
    Point2();

    /**
     * Constructor with initial values for x,y.
     * @param   ix   x coordinate position.
     * @param   iy   y coordinate position.
     */
    Point2(float ix, float iy);

    /**
     * Copy constructor.
     * @param   p   Point to copy to the new point.
     */
    Point2(const Point2 &p);

    /**
     * Convert a homogeneous coordinate into a cartesian coordinate.
     * @param   p   Homogenous point
     */
    Point2(const HPoint2 &p);

    /**
     * Assignment operator
     * @param   p   Point to assign to this point.
     * @return  Returns the address of this point.
     */
    Point2 &operator=(const Point2 &p);

    /**
     * Set the coordinate components to the specified values.
     * @param   ix   x coordinate position.
     * @param   iy   y coordinate position.
     */
    void set(float ix, float iy);

    /**
     * Equality operator.
     * @param   p  Point to compare to the current point.
     * @return  Returns true if two points are equal, false otherwise.
     */
    bool operator==(const Point2 &p) const;

    /**
     * Affine combination of this point with another point. 2 scalars are provided
     * (a0 and a1) and the must add to 1.
     * @param  a0  Scalar for this point
     * @param  a1  Scalar for p1
     * @param  p1  Point 1
     */
    Point2 affine_combination(float a0, float a1, const Point2 &p1) const;

    /**
     * Gets the midpoint on a line segment between this point and point p1.
     * @param  p1  Point
     * @return  Returns the midpoint between this point and p1.
     */
    Point2 mid_point(const Point2 &p1) const;

    /**
     * Test if point is inside polygon: Shoots a test ray along +x axis.
     * The strategy is to compare polygon vertex y values to the test point
     * y and quickly discard edges entirely to one side of the test ray.
     * @param  polygon  Polygon to test.
     * @return  Returns true if the point is inside the polygon, false if not.
     */
    bool is_in_polygon(const std::vector<Point2> &polygon) const;

    /**
     * Add a vector to the current point.
     * @param   v  Vector to add to the current point.
     * @return  Returns a new point: the result of the current point
     *          plus the specified vector.
     */
    Point2 operator+(const Vector2 &v) const;

    /**
     * Subtract a vector from the current point.
     * @param   v  Vector to subtract from the current point.
     * @return  Returns a new point: the result of the current point
     *          minus the specified vector.
     */
    Point2 operator-(const Vector2 &v) const;

    /**
     * Subtraction of a point from the current point.
     * @param   p to subtract from the current point.
     * @return  Returns a vector.
     */
    Vector2 operator-(const Point2 &p) const;
};

} // namespace cg

#endif
