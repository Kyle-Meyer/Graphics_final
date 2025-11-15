//============================================================================
//	Johns Hopkins University Engineering for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt
//	File:    noise.hpp
//	Purpose: Noise generation methods.
//============================================================================

#ifndef __GEOMETRY_NOISE_HPP__
#define __GEOMETRY_NOISE_HPP__

#include "geometry/point3.hpp"

namespace cg
{

// NOTE - not required until 605.767!

/**
 * Noise generation methods
 */
class Noise
{
  public:
    /**
     * Constructor
     */
    Noise();

    /**
     * Finds the noise at a specific 3D position. Linearly interpolates
     * lattice noise.
     * @param  p       Position
     * @param  scale   Scale
     * @return  Returns linearly interpolated noise value.
     */
    float noise(const Point3 &p, float scale);

    /**
     * Find turbelence value.
     * @param  scale   Scale
     * @param  p       Position
     * @return  Returns a turbulence value
     */
    float turbulence(float scale, const Point3 &p);
};

} // namespace cg

#endif
