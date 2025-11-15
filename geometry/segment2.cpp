#include "geometry/segment2.hpp"

#include "geometry/geometry.hpp"

#include <cmath>

namespace cg
{

namespace
{
// Clipping code words for Cohen-Sutherland line segment clipping
union ClipCode
{
    uint8_t code;
    struct
    {
        uint8_t left : 1;
        uint8_t right : 1;
        uint8_t bottom : 1;
        uint8_t top : 1;
    } m_bits;

    // Constructor. Set code to 0;
    ClipCode(void) : code(0) {}
};
} // namespace

LineSegment2::LineSegment2() : a{0.0f, 0.0f}, b{0.0f, 0.0f} {}

LineSegment2::LineSegment2(const Point2 &p1, const Point2 &p2) : a(p1), b(p2) {}

Segment2PointDistanceResult LineSegment2::distance(const Point2 &p) const
{
    // Construct vectors v (ab) and w (ap)
    Vector2 v(a, b);
    Vector2 w(a, p);

    // Numerator of the component of w onto v. If <= 0 then a
    // is the closest point. By separat_ing into the numerator
    // and denominator of the component we avoid a division unless
    // it is necessary.
    float n = w.dot(v);
    if(n <= 0.0f) { return {w.norm(), a}; }

    // Get the denominator of the component.  If the component >= 1
    // (d <= n) then point b is the closest point
    float d = v.dot(v);
    if(d <= n) { return {Vector2(b, p).norm(), b}; }

    // Closest point is along the segment. The point is the projection of
    // w onto v.
    Point2 closest = a + v * (n / d);
    return {Vector2(closest, p).norm(), closest};
}

Segment2IntersectionResult LineSegment2::intersect(const LineSegment2 &segment) const
{
    // Construct vectors
    Vector2 v = b - a;
    Vector2 w = segment.b - segment.a;

    // Set 2D perpendicular vector to w
    Vector2 wp(w.y, -w.x);

    // Check if denominator will be 0 (lines are parallel)
    float wp_dot_v = wp.dot(v);
    if(wp_dot_v == 0.0f) { return {false, Point2()}; }

    // Solve for the parameter t. Early exit if no intersect.
    Vector2 c = segment.a - a;
    float   t = wp.dot(c) / wp_dot_v;
    if(t < 0.0f || t > 1.0f) { return {false, Point2()}; }

    // Solve for the parameter u
    Vector2 vp(v.y, -v.x);
    float   u = vp.dot(c) / wp_dot_v;
    if(u < 0.0f || u > 1.0f) { return {false, Point2()}; }

    // An intersect occurs. Return 'true' and the intersect point.
    return {true, a + v * t};
}

Segment2ClipResult LineSegment2::clip_to_polygon(const std::vector<Point2> &poly) const
{
    // Initialize the candidate interval
    float t_out = 1.0f;
    float t_in = 0.0f;

    // Ray for this line segment
    Vector2 c = b - a;

    float   t_hit;
    Vector2 n;
    Vector2 w;
    auto    pt1 = poly.end() - 1;
    auto    pt2 = poly.begin();
    for(; pt2 != poly.end(); pt1 = pt2, pt2++)
    {
        // Set an outward facing normal (polygon is assumed to be CCW)
        n.set(pt2->y - pt1->y, pt1->x - pt2->x);

        // Get the dot product of the normal to this polygon edge
        // with the ray
        float n_dot_c = n.dot(c);

        // Check for parallel line
        if(std::abs(n_dot_c) < EPSILON) { continue; }

        w = *pt1 - a;
        t_hit = n.dot(w) / n_dot_c;

        // Ray is exiting P
        if(n_dot_c > 0) t_out = (t_out < t_hit) ? t_out : t_hit;
        else t_in = (t_in > t_hit) ? t_in : t_hit;

        // Early out
        if(t_in > t_out)
        {
            // Set clip segment to 0s
            LineSegment2 clip_segment;
            clip_segment.a.set(0.0f, 0.0f);
            clip_segment.b.set(0.0f, 0.0f);
            return {false, clip_segment};
        }
    }
    // If candidate interval is not empty then set the clip segment
    LineSegment2 clip_segment;
    clip_segment.a = a + c * t_in;
    clip_segment.b = a + c * t_out;

    return {true, clip_segment};
}

Segment2ClipResult LineSegment2::clip_to_rectangle(const CRectangle &r) const
{
    ClipCode     p1, p2;
    LineSegment2 clip;
    clip.a = a;
    clip.b = b;
    while(true)
    {
        // Form code words for the two points
        p1.m_bits.left = (clip.a.x < r.left);
        p1.m_bits.right = (clip.a.x > r.right);
        p1.m_bits.bottom = (clip.a.y < r.bottom);
        p1.m_bits.top = (clip.a.y > r.top);
        p2.m_bits.left = (clip.b.x < r.left);
        p2.m_bits.right = (clip.b.x > r.right);
        p2.m_bits.bottom = (clip.b.y < r.bottom);
        p2.m_bits.top = (clip.b.y > r.top);

        // Trivial accept - both points are inside, done
        if(p1.code == 0 && p2.code == 0) return {true, clip};

        // Trivial reject. Both points are outside
        if((p1.m_bits.left == 1 && p2.m_bits.left == 1) ||
           (p1.m_bits.right == 1 && p2.m_bits.right == 1) ||
           (p1.m_bits.bottom == 1 && p2.m_bits.bottom == 1) ||
           (p1.m_bits.top == 1 && p2.m_bits.top == 1))
        {
            clip.a.set(0.0f, 0.0f);
            clip.b.set(0.0f, 0.0f);
            return {false, clip};
        }
        if(p1.code != 0)
        {
            if(p1.m_bits.left)
            {
                clip.a.y += (r.left - clip.a.x) * (clip.a.y - clip.b.y) / (clip.a.x - clip.b.x);
                clip.a.x = r.left;
            }
            else if(p1.m_bits.right)
            {
                clip.a.y += (r.right - clip.a.x) * (clip.a.y - clip.b.y) / (clip.a.x - clip.b.x);
                clip.a.x = r.right;
            }
            else if(p1.m_bits.bottom)
            {
                clip.a.x += (clip.a.y - r.bottom) * (clip.b.x - clip.a.x) / (clip.a.y - clip.b.y);
                clip.a.y = r.bottom;
            }
            else if(p1.m_bits.top)
            {
                clip.a.x += (clip.a.y - r.top) * (clip.b.x - clip.a.x) / (clip.a.y - clip.b.y);
                clip.a.y = r.top;
            }
        }
        else
        {
            if(p2.m_bits.left)
            {
                clip.b.y += (r.left - clip.b.x) * (clip.b.y - clip.a.y) / (clip.b.x - clip.a.x);
                clip.b.x = r.left;
            }
            else if(p2.m_bits.right)
            {
                clip.b.y += (r.right - clip.b.x) * (clip.b.y - clip.a.y) / (clip.b.x - clip.a.x);
                clip.b.x = r.right;
            }
            else if(p2.m_bits.bottom)
            {
                clip.b.x += (clip.b.y - r.bottom) * (clip.a.x - clip.b.x) / (clip.b.y - clip.a.y);
                clip.b.y = r.bottom;
            }
            else if(p2.m_bits.top)
            {
                clip.b.x += (clip.b.y - r.top) * (clip.a.x - clip.b.x) / (clip.b.y - clip.a.y);
                clip.b.y = r.top;
            }
        }
    }

    return {true, clip};
}

} // namespace cg
