#include "geometry/types.hpp"

namespace cg
{

VertexAndNormal::VertexAndNormal()
{
    vertex.set(0.0f, 0.0f, 0.0f);
    normal.set(0.0f, 0.0f, 0.0f);
}

VertexAndNormal::VertexAndNormal(const Point3 &v)
{
    vertex = v;
    normal.set(0.0f, 0.0f, 0.0f);
}

} // namespace cg
