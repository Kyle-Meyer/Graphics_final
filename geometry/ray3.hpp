//============================================================================
//	Johns Hopkins University Engineering for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt
//	File:    ray3.hpp
//	Purpose: Ray class. Includes intersection methods with other objects.
//============================================================================

#ifndef __GEOMETRY_RAY3_HPP__
#define __GEOMETRY_RAY3_HPP__

#include "geometry/aabb.hpp"
#include "geometry/bounding_sphere.hpp"
#include "geometry/plane.hpp"
#include "geometry/point3.hpp"
#include "geometry/types.hpp"
#include "geometry/vector3.hpp"

#include <cstdint>
#include <vector>

namespace cg
{

// Forward Declarations
struct RayRefractionResult;
struct RayObjectIntersectResult;
struct RayTriangleIntersectResult;
struct RayMeshIntersectResult;

/**
 * 3D ray
 */
struct Ray3
{
    Point3  o; // Origin of the ray
    Vector3 d; // Ray direction

    /**
     * Default constructor.
     */
    Ray3();

    /**
     * Constructor given 2 points.
     * @param   p1    Start point of the ray.
     * @param   p2    End point of the ray (at t = 1).
     * @param   normalize  If true normalize the direction vector
     */
    Ray3(const Point3 &p1, const Point3 &p2, bool normalize);

    /**
     * Constructor given an origin and a direction.
     * @param   origin   Origin point of the ray.
     * @param   dir      Ray direction.
     */
    Ray3(const Point3 &origin, const Vector3 &dir);

    /**
     * Constructor given an origin and a direction. Normalizes the
     * ray direction.
     * @param   origin   Origin point of the ray.
     * @param   dir      Ray direction.
     */
    Ray3(const Point3 &origin, const Vector3 &dir, bool normalize);

    /**
     * Calculates the reflected ray given the intersect point and
     * the normal to the surface.
     * @param   int_pt  Intersection point (origin of the reflected ray)
     * @param   n       Normal at the intersection point
     * @return  Returns the reflected ray.
     */
    Ray3 reflect(const Point3 &int_pt, const Vector3 &n) const;

    /**
     * Finds the refracted ray.
     * @param   int_pt  Intersection point (origin of the refracted ray)
     * @param   n       Normal at the intersection point
     * @param   u1      Index of refraction (leaving)
     * @param   u2      Index of refraction (entering)
     * @return  Returns the refracted ray and if total internal reflection.
     */
    RayRefractionResult refract(const Point3 &int_pt, Vector3 &n, float u1, float u2) const;

    /**
     * Returns the intersect point given the parameter t
     * @param   t    Parameter along the ray
     * @return  Returns the point along the direction d from the origin a.
     */
    Point3 intersect(float t) const;

    //  ---------------- Intersection methods --------------------//

    /**
     * Intersection of the ray with a plane.
     * @param  p  Plane to test intersection with
     * @return Returns whether or not there is an intersection, and the distance
     *         at which the intersection occurs (0.0 if no intersection).
     */
    RayObjectIntersectResult intersect(const Plane &p) const;

    /**
     * Intersection of a ray and a sphere using vector method. For efficiency
     * assume the ray is unit length (we will construct it with a unit length
     * vector)
     * @param  sphere  Sphere to test intersection with
     * @return Returns whether or not there is an intersection, and the distance
     *         at which the intersection occurs (0.0 if no intersection).
     */
    RayObjectIntersectResult intersect(const BoundingSphere &sphere) const;

    /**
     * Intersection of a ray and an axis aligned bounding box.
     * @param  box  AABB to test intersection with
     * @return Returns whether or not there is an intersection, and the distance
     *         at which the intersection occurs (0.0 if no intersection).
     */
    RayObjectIntersectResult intersect(const AABB &box) const;

    /**
     * Intersection of a ray and a polygon
     * @param  polygon  Polygon to test intersection with
     * @param  normal   Normal to the polygon
     * @return Returns whether or not there is an intersection, and the distance
     *         at which the intersection occurs (0.0 if no intersection).
     */
    RayObjectIntersectResult intersect(const std::vector<Point3> &polygon,
                                       const Vector3             &normal) const;

    /**
     * Intersection of a ray with a triangle.
     * @param   v0  Vertex of the triangle
     * @param   v1  Vertex of the triangle
     * @param   v2  Vertex of the triangle
     * @return Returns whether or not there is an intersection, the distance
     *         at which the intersection occurs (0.0 if no intersection), and
     *         the barycentric coordinates of intersection.
     */
    RayTriangleIntersectResult
        intersect(const Point3 &v0, const Point3 &v1, const Point3 &v2) const;

    /**
     * Does an intersection exist between ray with a triangle.
     * @param  v0  Vertex 0 of the triangle
     * @param  v1  Vertex 1 of the triangle
     * @param  v2  Vertex 2 of the triangle
     * @return Returns true if an intersection exists, false if not.
     */
    bool does_intersect_exist(const Point3 &v0, const Point3 &v1, const Point3 &v2) const;

    /**
     * Calculates the intersect of a ray and a triangle mesh object.
     * Note that this returns the face index of the nearest intersection found as well
     * as the barycentric coordinates of the intersection. This allows the triangle and
     * its normal and texture coordinates to be recovered later.
     * @param vertex_list Vertex list of the triangle mesh.
     * @param face_list   Face index list.
     * @param t_min       Current minimum intersection (t) value along the ray.
     * @return Returns whether or not there is an intersection, the distance
     *         at which the intersection occurs (0.0 if no intersection),
     *         the barycentric coordinates of intersection, and the face index.
     */
    RayMeshIntersectResult intersect(const std::vector<Point3>   &vertex_list,
                                     const std::vector<uint16_t> &face_list,
                                     float                        t_min) const;

    /**
     * Does an intersection exist between ray and a triangle mesh. The intersection
     * must occur prior to t_min (intersection value t between 0 and t_min).
     * @param vertex_list Vertex list of the triangle mesh.
     * @param face_list   Face index list.
     * @param t_min       t value for intersection.
     * @return Returns true if an intersection exists, false if not.
     */
    bool does_intersect_exist(const std::vector<Point3>   &vertex_list,
                              const std::vector<uint16_t> &face_list,
                              float                        t_min) const;

    /**
     * Does an intersection exist between ray and a triangle mesh. The intersection
     * must occur prior to t_min (intersection value t between 0 and t_min).
     * @param vertex_list Vertex list of the triangle mesh.
     * @param face_list   Face index list.
     * @param t_min       t value for intersection.
     * @return Returns true if an intersection exists, false if not.
     */
    bool does_intersect_exist(const std::vector<VertexAndNormal> &vertex_list,
                              const std::vector<uint16_t>        &face_list,
                              float                               t_min) const;
};

struct RayRefractionResult
{
    Ray3 refracted_ray;
    bool total_internal_refraction;
};

struct RayObjectIntersectResult
{
    bool  intersects;
    float distance;
};

struct RayTriangleIntersectResult
{
    bool  intersects;
    float distance;
    float barycentric_u;
    float barycentric_v;
};

struct RayMeshIntersectResult
{
    bool     intersects;
    float    distance;
    float    barycentric_u;
    float    barycentric_v;
    uint32_t face_index;
};

} // namespace cg

#endif
