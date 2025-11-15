#include "geometry/hpoint3.hpp"

#include "geometry/geometry.hpp"

#include <cmath>

namespace cg
{

HPoint3::HPoint3() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}

HPoint3::HPoint3(float ix, float iy, float iz, float iw) : x(ix), y(iy), z(iz), w(iw) {}

Point3 HPoint3::to_cartesian() const
{
    if(w == 1.0f) { return Point3(x, y, z); }
    else
    {
        // Perform division through by w
        float d = (std::abs(w) > EPSILON) ? (1.0f / w) : 1.0f;
        return Point3(x * d, y * d, z * d);
    }
}

} // namespace cg
