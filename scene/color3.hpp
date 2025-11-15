///============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt
//	File:    color3.hpp
//	Purpose: Color structure with RGB. Supports adding and blending colors.
//           Includes a method to clamp to [0,1] range.
//============================================================================

#ifndef __SCENE_COLOR3_HPP__
#define __SCENE_COLOR3_HPP__

#include <cstdint>

namespace cg
{

// Forward Declaration
struct Color4;

/**
 * Color structure. Colors are specified as red, green, and blue values
 * with range [0.0-1.0].
 * Note: methods no longer clamp to [0,1] range. It is the responsibility
 * of the app to call Clamp when needed. This is an efficiency consideration
 * to support ray-tracing.
 */
struct Color3
{
    float r, g, b;

    /**
     * Constructor.  Values default to 0,0,0.
     */
    Color3();

    /**
     * Constructor.  Set RGB to specified values. Clamps to range [0.0, 1.0]
     * @param	red		Red intensity
     * @param	green		Green intensity
     * @param	blue		Blue intensity
     */
    Color3(float red, float green, float blue);

    /**
     * Copy constructor.
     * @param	c	Color assigned to member.
     */
    Color3(const Color3 &c);

    /**
     * Copy constructor given an RGBA color (ignores alpha).
     * @param	c	Color assigned to member.
     */
    Color3(const Color4 &c);

    /**
     * Assignment operator.
     * @param	c	Color to assign to the object.
     * @return	Returns the address of the member data.
     */
    Color3 &operator=(const Color3 &c);

    /**
     *	Set the color to the specified RGB values.
     * @param	ir		Red intensity
     * @param	ig		Green intensity
     * @param	ib		Blue intensity
     */
    void set(float ir, float ig, float ib);

    /**
     * Get the red value in the range 0-255
     */
    uint8_t r_byte() const;

    /**
     * Get the green value in the range 0-255
     */
    uint8_t g_byte() const;

    /**
     * Get the blue value in the range 0-255
     */
    uint8_t b_byte() const;

    /**
     * Multiplication operator: Multiplies the color by another color
     */
    Color3 operator*(const Color3 &color) const;

    /**
     * Multiplication operator: Multiplies the color by an RGBA color.
     * Ignores alpha.
     * @return  Returns RGB color
     */
    Color3 operator*(const Color4 &color) const;

    /**
     * Scales the color by a scalar factor.
     */
    Color3 operator*(float factor);

    /**
     * Adds another color to the current color. Clamps to the valid range.
     */
    Color3 &operator+=(const Color3 &color);

    /**
     * Creates a new color that is the current color plus the
     * specified color.
     * @param   c  Color to add to the current color.
     * @return  Returns the resulting color.
     */
    Color3 operator+(const Color3 &c) const;

    // Clamps a color to the range [0.0, 1.0]
    void clamp();
};

} // namespace cg

#endif
