
#include "scene/color3.hpp"

#include "scene/color4.hpp"

#include <algorithm>

namespace cg
{

Color3::Color3() : r(0.0f), g(0.0f), b(0.0f) {}

Color3::Color3(float red, float green, float blue) : r(red), g(green), b(blue) {}

Color3::Color3(const Color3 &c) : r(c.r), g(c.g), b(c.b) {}

Color3::Color3(const Color4 &c) : r(c.r), g(c.g), b(c.b) {}

Color3 &Color3::operator=(const Color3 &c)
{
    r = c.r;
    g = c.g;
    b = c.b;
    return *this;
}

void Color3::set(float ir, float ig, float ib)
{
    r = ir;
    g = ig;
    b = ib;
}

uint8_t Color3::r_byte() const { return static_cast<uint8_t>(r * 255.0f); }

uint8_t Color3::g_byte() const { return static_cast<uint8_t>(g * 255.0f); }

uint8_t Color3::b_byte() const { return static_cast<uint8_t>(b * 255.0f); }

Color3 Color3::operator*(const Color3 &color) const
{
    return Color3(r * color.r, g * color.g, b * color.b);
}

Color3 Color3::operator*(const Color4 &color) const
{
    return Color3(r * color.r, g * color.g, b * color.b);
}

Color3 Color3::operator*(const float factor) { return Color3(r * factor, g * factor, b * factor); }

Color3 &Color3::operator+=(const Color3 &color)
{
    r += color.r;
    g += color.g;
    b += color.b;
    return *this;
}

Color3 Color3::operator+(const Color3 &c) const { return Color3(r + c.r, g + c.g, b + c.b); }

void Color3::clamp()
{
    r = std::min(std::max(r, 0.0f), 1.0f);
    g = std::min(std::max(g, 0.0f), 1.0f);
    b = std::min(std::max(b, 0.0f), 1.0f);
}

} // namespace cg
