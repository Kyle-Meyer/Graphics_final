#include "geometry/plane.hpp"

#include "geometry/geometry.hpp"

#include <cmath>

namespace cg
{

Plane::Plane() : a(0.0f), b(0.0f), c(0.0f), d(0.0f) {}

Plane::Plane(const Point3 &p, const Vector3 &n) { set(p, n); }

Plane::Plane(const Point3 &p1, const Point3 &p2, const Point3 &p3)
{
    // Construct 2 vectors and find the cross product.
    Vector3 v1(p1, p2);
    Vector3 v2(p1, p3);
    set(p1, v1.cross(v2));
}

void Plane::set(const Point3 &p, const Vector3 &n)
{
    a = n.x;
    b = n.y;
    c = n.z;
    d = (a * p.x + b * p.y + c * p.z);
}

void Plane::normalize()
{
    float l = std::sqrt(a * a + b * b + c * c);
    float m = (std::abs(l) > EPSILON) ? 1.0f / l : 1.0f;
    a *= m;
    b *= m;
    c *= m;
    d *= m;
}

float Plane::solve(const Point3 &p) const { return (a * p.x + b * p.y + c * p.z - d); }

Vector3 Plane::get_normal() const { return Vector3(a, b, c); }

} // namespace cg
