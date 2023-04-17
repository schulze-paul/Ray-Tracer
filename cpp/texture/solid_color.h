#ifndef SOLID_COLOR_H
#define SOLID_COLOR_H

#include "texture.h"

class SolidColor : public Texture
{
public:
    SolidColor() {}
    SolidColor(Vec3 c) : color_value(c) {}
    SolidColor(double red, double green, double blue) : SolidColor(Vec3(red, green, blue)) {}

    virtual Color value(double u, double v, const Vec3 &p) const override
    {
        return color_value;
    }

private:
    Color color_value;
};

#endif // SOLID_COLOR_H