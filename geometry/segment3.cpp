#include "geometry/segment3.hpp"

#include "geometry/geometry.hpp"

namespace cg
{

LineSegment3::LineSegment3(const Point3 &p1, const Point3 &p2) : a(p1), b(p2) {}

Segment3PointDistanceResult LineSegment3::distance(const Point3 &p) const
{
    // Construct vectors v (ab) and w (ap)
    Vector3 v(a, b);
    Vector3 w(a, p);

    // Numerator of the component of w onto v. If <= 0 then a
    // is the closest point. By separating into the numerator
    // and denominator of the component we avoid a division unless
    // it is necessary.
    float n = w.dot(v);
    if(n <= 0.0f) { return {w.norm(), a}; }

    // Get the denominator of the component.  If the component >= 1
    // (d <= n) then point b is the closest point
    float d = v.dot(v);
    if(d <= n) { return {Vector3(b, p).norm(), b}; }

    // Closest point is along the segment. The point is the projection of
    // w onto v.
    Point3 closest = a + v * (n / d);
    return {Vector3(closest, p).norm(), closest};
}

} // namespace cg
