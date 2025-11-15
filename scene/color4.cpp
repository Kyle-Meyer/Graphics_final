#include "scene/color4.hpp"

#include <algorithm>

namespace cg
{

Color4::Color4(void) : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}

Color4::Color4(float red, float green, float blue, float alpha) :
    r(red), g(green), b(blue), a(alpha)
{
}

Color4::Color4(float red, float green, float blue) : r(red), g(green), b(blue), a(1.0f) {}

Color4::Color4(const Color3 &c) : r(c.r), g(c.g), b(c.b), a(1.0f) {}

Color4::Color4(const Color4 &c) : r(c.r), g(c.g), b(c.b), a(c.a) {}

Color4 &Color4::operator=(const Color4 &c)
{
    r = c.r;
    g = c.g;
    b = c.b;
    a = c.a;
    return *this;
}

void Color4::set(float ir, float ig, float ib, float ia)
{
    r = ir;
    g = ig;
    b = ib;
    a = ia;
}

uint8_t Color4::r_byte() const { return static_cast<uint8_t>(r * 255.0f); }

uint8_t Color4::g_byte() const { return static_cast<uint8_t>(g * 255.0f); }

uint8_t Color4::b_byte() const { return static_cast<uint8_t>(b * 255.0f); }

uint8_t Color4::a_byte() const { return static_cast<uint8_t>(a * 255.0f); }

Color4 Color4::operator*(const Color4 &color) const
{
    return Color4(r * color.r, g * color.g, b * color.b, a * color.a);
}

Color3 Color4::operator*(const Color3 &color) const
{
    return Color3(r * color.r, g * color.g, b * color.b);
}

Color4 Color4::operator*(float factor)
{
    return Color4(r * factor, g * factor, b * factor, a * factor);
}

Color4 &Color4::operator+=(const Color4 &color)
{
    r += color.r;
    g += color.g;
    b += color.b;
    a += color.a;
    return *this;
}

Color4 Color4::operator+(const Color4 &c) const
{
    return Color4(r + c.r, g + c.g, b + c.b, a + c.a);
}

void Color4::clamp()
{
    r = std::min(std::max(r, 0.0f), 1.0f);
    g = std::min(std::max(g, 0.0f), 1.0f);
    b = std::min(std::max(b, 0.0f), 1.0f);
    a = std::min(std::max(a, 0.0f), 1.0f);
}

} // namespace cg
