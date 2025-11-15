#include "geometry/bounding_sphere.hpp"

#include "geometry/geometry.hpp"

namespace cg
{

BoundingSphere::BoundingSphere() : center{0.0f, 0.0f, 0.0f}, radius(1.0f) {}

BoundingSphere::BoundingSphere(const BoundingSphere &s) : center(s.center), radius(s.radius) {}

BoundingSphere::BoundingSphere(const Point3 &c, float r) : center(c), radius(r) {}

BoundingSphere::BoundingSphere(std::vector<Point3> &vertex_list)
{
    // Complete in 605.767
}

BoundingSphere &BoundingSphere::merge_with(const BoundingSphere &s2)
{
    // Complete in 605.767
    return *this;
}

BoundingSphere BoundingSphere::merge(const BoundingSphere &s2) const
{
    BoundingSphere combined(*this);
    combined.merge_with(s2);
    return combined;
}

} // namespace cg
