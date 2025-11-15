#include "geometry/aabb.hpp"

#include "geometry/geometry.hpp"

namespace cg
{

// NOTE - this is not required until 605.767!

AABB::AABB()
{
    // Complete in 605.767
}

AABB::AABB(const Point3 &min, const Point3 &max)
{
    // Complete in 605.767
}

AABB::AABB(const std::vector<Point3> &vertex_list)
{
    // Complete in 605.767
}

void AABB::create(const std::vector<Point3> &vertex_list)
{
    // Complete in 605.767
}

void AABB::update(const Point3 &min, const Point3 &max)
{
    // Complete in 605.767
}

void AABB::merge(const AABB &box)
{
    // Complete in 605.767
}

Point3 AABB::min_pt() const
{
    // Complete in 605.767
    return Point3();
}

Point3 AABB::max_pt() const
{
    // Complete in 605.767
    return Point3();
}

void compute_center()
{
    // Complete in 605.767
}

} // namespace cg
