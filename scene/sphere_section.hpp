//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:	 David W. Nesbitt
//	File:    sphere_section.hpp
//	Purpose: Scene graph geometry node representing a section of a
//           sphere.
//
//============================================================================

#ifndef __SCENE_SPHERE_SECTION_HPP__
#define __SCENE_SPHERE_SECTION_HPP__

#include "scene/tri_surface.hpp"

namespace cg
{

class SphereSection : public TriSurface
{
  public:
    /**
     * Creates a section of a sphere with bounds given by min_lat, max_lat and
     * min_lon, max_lon. The number of subdivisions of each is also given. Can be
     * used to construct a full sphere. This method uses a single vertex at the
     * north and south pole and avoids creating extra triangles for polar regions.
     * @param   min_lat   Minimum latitude
     * @param   max_lat   Maximum latitude
     * @param   num_lat     Number of divisions of latitude
     * @param   min_lon   Minimum longitude
     * @param   max_lon   Maximum longitude
     * @param   num_lon     Number of divisions of longitude
     * @param   radius   Radius of the sphere
     */
    SphereSection(float    min_lat,
                  float    max_lat,
                  uint32_t num_lat,
                  float    min_lon,
                  float    max_lon,
                  uint32_t num_lon,
                  float    radius,
                  int32_t  position_loc,
                  int32_t  normal_loc);

    /**
     * 
     * @param   min_lat       Minimum latitude
     * @param   max_lat       Maximum latitude
     * @param   num_lat       Number of divisions of latitude
     * @param   min_lon       Minimum longitude
     * @param   max_lon       Maximum longitude
     * @param   num_lon       Number of divisions of longitude
     * @param   radius        Radius of the sphere
     * @param   position_loc  Position attribute location
     * @param   normal_loc    Normal attribute location
     * @param   texcoord_loc  Texture coordinate attribute location
     */
    SphereSection(float    min_lat,
                  float    max_lat,
                  uint32_t num_lat,
                  float    min_lon,
                  float    max_lon,
                  uint32_t num_lon,
                  float    radius,
                  int32_t  position_loc,
                  int32_t  normal_loc,
                  int32_t  texcoord_loc);

  private:
    // Make default constructor private to force use of the constructor
    // with number of subdivisions.
    SphereSection();
};

} // namespace cg

#endif
