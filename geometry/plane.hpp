//============================================================================
//	Johns Hopkins University Engineering for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt
//	File:    plane.hpp
//	Purpose: Structures to hold definition of a plane in 3 space.
//============================================================================

#ifndef __GEOMETRY_PLANE_HPP__
#define __GEOMETRY_PLANE_HPP__

#include "geometry/point3.hpp"
#include "geometry/vector3.hpp"

namespace cg
{

struct Plane
{
    float a, b, c, d; // Components of the plane equation

    /**
     * Default constructor
     */
    Plane();

    /**
     * Constructor given a normal vector and a point on the plane.
     * @param  p  Point on the plane
     * @param  n  Normal to the plane
     */
    Plane(const Point3 &p, const Vector3 &n);

    /**
     * Constructor given 3 points on the plane. Assume the points are
     * in counter-clockwise order to determine the outward direction.
     * @param  p1   1st point on the plane
     * @param  p2   2nd point on the plane
     * @param  p3   3rd point on the plane
     */
    Plane(const Point3 &p1, const Point3 &p2, const Point3 &p3);

    /**
     * Set the plane given a normal vector and a point on the plane
     * @param  p  Point on the plane
     * @param  n  Normal to the plane
     */
    void set(const Point3 &p, const Vector3 &n);

    /**
     * Normalize the plane equation - divides through by the magnitude of the
     * normal vector to make sure the normal is unit length.
     */
    void normalize();

    /**
     * Solves the plane equation using the supplied point. The sign of the
     * value indicates on which side of the plane the point lies. If the plane
     * is constructed with a unit normal, then the value is the signed distance
     * of the point to the plane.
     * @param  p  Point to solve the plane equation with.
     * @return  Returns the solution of the plane equation given an input point
     */
    float solve(const Point3 &p) const;

    /**
     * Returns a normal to the plane
     * @return  Returns a normal to the plane.
     */
    Vector3 get_normal() const;
};

} // namespace cg

#endif
