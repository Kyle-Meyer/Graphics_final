///============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.667 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	Brian Russin
//
//	Author:  David W. Nesbitt
//	File:    color4.hpp
//	Purpose: Color structure with RGBA. Supports adding and blending colors.
//           Includes clamping to [0,1] range.
//============================================================================

#ifndef __SCENE_COLOR4_HPP__
#define __SCENE_COLOR4_HPP__

#include "scene/color3.hpp"

#include <cstdint>

namespace cg
{

/**
 * Color structure. Colors are specified as red, green, and blue values
 * with range [0.0-1.0].
 * Note: methods no longer clamp to [0,1] range. It is the responsibility
 * of the app to call Clamp when needed. This is an efficiency consideration
 * to support ray-tracing.
 */
struct Color4
{
    float r, g, b, a;

    /**
     * Constructor.  Values default to 0,0,0.
     */
    Color4(void);

    /**
     * Constructor. Set RGB to specified values. Clamps to range [0.0, 1.0]
     * @param	red     Red intensity
     * @param	green   Green intensity
     * @param	blue    Blue intensity
     * @param	alpha   Alpha value for blending
     */
    Color4(float red, float green, float blue, float alpha);

    /**
     * Constructor with RGB. Sets A to 1.0. Clamps to range [0.0, 1.0]
     * @param	red     Red intensity
     * @param	green   Green intensity
     * @param	blue    Blue intensity
     */
    Color4(float red, float green, float blue);

    /**
     * Constructor from a Color3. Sets A to 1.0f. Should be no need to clamp
     * since Color3 must have been clamped to [0,1] range.
     * @param c Color assigned to member.
     */
    Color4(const Color3 &c);

    /**
     * Copy constructor.
     * @param c Color assigned to member.
     */
    Color4(const Color4 &c);

    /**
     * Assignment operator.
     * @param  c Color to assign to the object.
     * @return Returns the address of the member data.
     */
    Color4 &operator=(const Color4 &c);

    /**
     *	Set the color to the specified RGB values.
     * @param	ir		Red intensity
     * @param	ig		Green intensity
     * @param	ib		Blue intensity
     */
    void set(float ir, float ig, float ib, float ia);

    /**
     * Get the red value in the range 0-255
     * @return  Returns red value as a [0-255] value
     */
    uint8_t r_byte() const;

    /**
     * Get the green value in the range 0-255
     * @return  Returns green value as a [0-255] value
     */
    uint8_t g_byte() const;

    /**
     * Get the blue value in the range 0-255
     * @return  Returns blue value as a [0-255] value
     */
    uint8_t b_byte() const;

    /**
     * Get the alpha value in the range 0-255
     * @return  Returns alpha value as a [0-255] value
     */
    uint8_t a_byte() const;

    /**
     * Multiplication operator: Multiplies the color by another color
     */
    Color4 operator*(const Color4 &color) const;

    /**
     * Multiplication operator: Multiplies the color by another color (RGB only).
     * Ignores alpha.
     * @return  Returns RGB color
     */
    Color3 operator*(const Color3 &color) const;

    /**
     * Scales the color by a scalar factor.
     */
    Color4 operator*(float factor);

    /**
     * Adds another color to the current color. Clamps to the valid range.
     */
    Color4 &operator+=(const Color4 &color);

    /**
     * Creates a new color that is the current color plus the
     * specified color.
     * @param   c  Color to add to the current color.
     * @return  Returns the resulting color.
     */
    Color4 operator+(const Color4 &c) const;

    /**
     * Clamps a color to the range [0.0, 1.0].
     */
    void clamp();
};

} // namespace cg

#endif
