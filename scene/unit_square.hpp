//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:	 David W. Nesbitt
//	File:    unit_square.hpp
//	Purpose: Scene graph geometry node representing a subdivided unit square.
//
//============================================================================

#ifndef __SCENE_UNIT_SQUARE_HPP__
#define __SCENE_UNIT_SQUARE_HPP__

#include "scene/tri_surface.hpp"

namespace cg
{

class UnitSquareSurface : public TriSurface
{
  public:
    /**
     * Creates a unit length and width "flat surface".  The surface is composed of
     * triangles such that the unit length/width surface is divided into n
     * equal paritions in both x and y. Constructs a vertex list and face list
     * for the surface.
     * @param  n   Number of subdivisions in x and y
     */
    UnitSquareSurface(uint32_t n, int32_t position_loc, int32_t normal_loc);

    /**
     * Creates a unit length and width "flat surface" with texture coordinates.
     * The surface is composed of triangles such that the unit length/width 
     * surface is divided into n equal paritions in both x and y. Constructs a 
     * vertex list and face list for the surface with texture coordinates.
     * @param  n              Number of subdivisions in x and y
     * @param  position_loc   Location of position attribute in shader
     * @param  normal_loc     Location of normal attribute in shader
     * @param  tex_coord_loc  Location of texture coordinate attribute in shader
     */
    UnitSquareSurface(uint32_t n, int32_t position_loc, int32_t normal_loc, int32_t tex_coord_loc);

    /**
     * Creates a unit length and width "flat surface" with texture coordinates and scaling.
     * The surface is composed of triangles such that the unit length/width 
     * surface is divided into n equal paritions in both x and y. Constructs a 
     * vertex list and face list for the surface with texture coordinates.
     * Texture coordinates are scaled to allow tiling/repeating of the texture.
     * @param  n              Number of subdivisions in x and y
     * @param  position_loc   Location of position attribute in shader
     * @param  normal_loc     Location of normal attribute in shader
     * @param  tex_coord_loc  Location of texture coordinate attribute in shader
     * @param  tex_scale      Texture coordinate scale factor (e.g., 10.0 for 10x10 tiling)
     */
    UnitSquareSurface(uint32_t n, int32_t position_loc, int32_t normal_loc, int32_t tex_coord_loc, float tex_scale);

  private:
    // Make default constructor private to force use of the constructor
    // with number of subdivisions.
    UnitSquareSurface();
};

} // namespace cg

#endif
