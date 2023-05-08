/*
Color.h
=======
Represents a color in RGB space.
*/

#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

/**
 * @brief      Class for color.
*/
class Color : public Vec3
{
public:
    Color() : Vec3() {}
    Color(double r, double g, double b) : Vec3(r, g, b) {}
    Color(const Vec3 &v) : Vec3(v) {}
    int write(std::ostream &out);
    inline double r() const { return e[0]; }
    inline double g() const { return e[1]; }
    inline double b() const { return e[2]; }
};

#endif // COLOR_H