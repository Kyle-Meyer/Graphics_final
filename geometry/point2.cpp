#include "geometry/point2.hpp"

#include "geometry/geometry.hpp"

namespace cg
{

Point2::Point2() : x(0.0f), y(0.0f) {}

Point2::Point2(float ix, float iy) : x(ix), y(iy) {}

Point2::Point2(const Point2 &p) : x(p.x), y(p.y) {}

Point2::Point2(const HPoint2 &p) { *this = p.to_cartesian(); }

Point2 &Point2::operator=(const Point2 &p)
{
    x = p.x;
    y = p.y;
    return *this;
}

void Point2::set(float ix, float iy)
{
    x = ix;
    y = iy;
}

bool Point2::operator==(const Point2 &p) const { return (x == p.x && y == p.y); }

Point2 Point2::affine_combination(float a0, float a1, const Point2 &p1) const
{
    return Point2(a0 * x + a1 * p1.x, a0 * y + a1 * p1.y);
}

Point2 Point2::mid_point(const Point2 &p1) const
{
    return Point2(0.5f * x + 0.5f * p1.x, 0.5f * y + 0.5f * p1.y);
}

bool Point2::is_in_polygon(const std::vector<Point2> &polygon) const
{
    bool inside = false;
    auto pt1 = polygon.end() - 1;
    auto pt2 = polygon.begin();
    bool y1 = (pt1->y >= y); //  Is vertex above or below test point
    bool y2;
    for(; pt2 != polygon.end(); pt1 = pt2, pt2++)
    {
        // Check if endpoints straddle ray. If so, +x ray could intersect
        // this edge.
        y2 = (pt2->y >= y);
        if(y1 != y2)
        {
            if(((pt2->y - y) * (pt1->x - pt2->x)) >= ((pt2->x - x) * (pt1->y - pt2->y)) == y2)
                inside = !inside;
        }
        y1 = y2;
    }
    return inside;
}

Point2 Point2::operator+(const Vector2 &v) const { return Point2(x + v.x, y + v.y); }

Point2 Point2::operator-(const Vector2 &v) const { return Point2(x - v.x, y - v.y); }

Vector2 Point2::operator-(const Point2 &p) const { return Vector2(x - p.x, y - p.y); }

} // namespace cg
