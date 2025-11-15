//============================================================================
//	Johns Hopkins University Engineering for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  Brian Russin
//	File:    structs.hpp
//	Purpose: Helper stucts to group vertex attributes
//
//============================================================================

#ifndef __GEOMETRY_TYPES_HPP__
#define __GEOMETRY_TYPES_HPP__

#include "geometry/point3.hpp"
#include "geometry/vector3.hpp"
#include "geometry/point2.hpp"
namespace cg
{

/**
 * Structure to hold a vertex position and normal
 */
struct VertexAndNormal
{
    Point3  vertex;
    Vector3 normal;

    VertexAndNormal();

    VertexAndNormal(const Point3 &v);
};

struct VertexNormalTexture
   {
       Point3  vertex;
       Vector3 normal;
       Point2  texcoord;

       VertexNormalTexture() : vertex(), normal(), texcoord() {}
       VertexNormalTexture(const Point3 &v) : vertex(v), normal(), texcoord() {}
       VertexNormalTexture(const Point3 &v, const Vector3 &n) : 
           vertex(v), normal(n), texcoord() {}
       VertexNormalTexture(const Point3 &v, const Vector3 &n, const Point2 &t) :
           vertex(v), normal(n), texcoord(t) {}
   };

} // namespace cg

#endif
