#include "geometry/ray3.hpp"

#include "geometry/geometry.hpp"

#include <cmath>

namespace cg
{

Ray3::Ray3() : o{0.0f, 0.0f, 0.0f}, d{1.0f, 0.0f, 0.0f} {}

Ray3::Ray3(const Point3 &p1, const Point3 &p2, bool normalize) : o(p1), d(p1, p2)
{
    if(normalize) d.normalize();
}

Ray3::Ray3(const Point3 &origin, const Vector3 &dir) : o(origin), d(dir) {}

Ray3::Ray3(const Point3 &origin, const Vector3 &dir, bool normalize)
{
    o = origin;
    d = dir;
    if(normalize) { d.normalize(); }
}

Ray3 Ray3::reflect(const Point3 &int_pt, const Vector3 &n) const
{
    // Required in 605.767
    return Ray3();
}

RayRefractionResult Ray3::refract(const Point3 &int_pt, Vector3 &n, float u1, float u2) const
{
    // Required in 605.767
    return RayRefractionResult{};
}

Point3 Ray3::intersect(const float t) const { return o + d * t; }

RayObjectIntersectResult Ray3::intersect(const Plane &p) const
{
    // Required in 605.767
    return RayObjectIntersectResult{false, 0.0f};
}
RayObjectIntersectResult Ray3::intersect(const BoundingSphere &sphere) const
{
    // Construct vector from ray origin to sphere center. Get squared length
    Vector3 l = sphere.center - o;
    float   l2 = l.norm_squared();

    // Component of l onto ray. Since the ray direction is unit length, the
    // component is the distance along the ray to the closest point to the
    // sphere (perp.)
    float s = l.dot(d);
    float r2 = sphere.radius * sphere.radius;
    if(s < 0.0f && l2 > r2)
    {
        // Early exit - no intersection. Sphere center is behind ray origin
        // AND ray origin is outside sphere
        return {false, 0.0f};
    }

    // Distance (squared) of closest point along the ray (perpendicular)
    float m2 = l2 - (s * s);
    if(m2 > r2)
    {
        // Ray passes outside the sphere
        return {false, 0.0f};
    }

    // Intersection occurs. Use Pythagorean theorem to find q - distance
    // from the nearest point along the ray to the intersection point
    if(l2 - r2 > EPSILON)
    {
        // Ray origin is outside sphere: nearest intersection is at
        // value t=s-q
        return {true, s - std::sqrt(r2 - m2)};
    }
    else
    {
        // Ray origin is inside sphere nearest intersection is at
        // value t=s+q
        return {true, s + std::sqrt(r2 - m2)};
    }
}

RayObjectIntersectResult Ray3::intersect(const AABB &box) const
{
    // Required in 605.767
    return {false, 0.0f};
}

RayObjectIntersectResult Ray3::intersect(const std::vector<Point3> &polygon,
                                         const Vector3             &normal) const
{
    // Required in 605.767
    return {false, 0.0f};
}

RayTriangleIntersectResult
    Ray3::intersect(const Point3 &v0, const Point3 &v1, const Point3 &v2) const
{
    // Required in 605.767
    return {false, 0.0f, 0.0f, 0.0f};
}

bool Ray3::does_intersect_exist(const Point3 &v0, const Point3 &v1, const Point3 &v2) const
{
    // Required in 605.767
    return false;
}

RayMeshIntersectResult Ray3::intersect(const std::vector<Point3>   &vertex_list,
                                       const std::vector<uint16_t> &face_list,
                                       float                        t_min) const
{
    // Required in 605.767
    return {false, 0.0f, 0.0f, 0.0f, 0};
}

bool Ray3::does_intersect_exist(const std::vector<Point3>   &vertex_list,
                                const std::vector<uint16_t> &face_list,
                                float                        t_min) const
{
    // Required in 605.767
    return false;
}

bool Ray3::does_intersect_exist(const std::vector<VertexAndNormal> &vertex_list,
                                const std::vector<uint16_t>        &face_list,
                                float                               t_min) const
{
    // Required in 605.767
    return false;
}

} // namespace cg
