#include "geometry/hpoint2.hpp"

#include "geometry/geometry.hpp"

#include <cmath>

namespace cg
{

HPoint2::HPoint2() : x(0.0f), y(0.0f), w(1.0f) {}

HPoint2::HPoint2(float ix, float iy, float iw) : x(ix), y(iy), w(iw) {}

Point2 HPoint2::to_cartesian() const
{

    if(w == 1.0f) { return Point2(x, y); }
    else
    {
        // Perform division through by w
        float d = (std::abs(w) > EPSILON) ? (1.0f / w) : 1.0f;
        return Point2(x * d, y * d);
    }
}

} // namespace cg
