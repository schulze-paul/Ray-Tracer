#ifndef TEXTURE_H
#define TEXTURE_H

#include "vec3.h"
#include "color.h"
#include "perlin.h"

class Texture
{
public:
    virtual Color value(double u, double v, const Vec3 &p) const = 0;
};

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

class CheckerTexture : public Texture
{
public:
    CheckerTexture() {}
    CheckerTexture(Texture *t0, Texture *t1) : even(t0), odd(t1) {}

    virtual Color value(double u, double v, const Vec3 &p) const override
    {
        auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        if (sines < 0)
        {
            return odd->value(u, v, p);
        }
        else
        {
            return even->value(u, v, p);
        }
    }

private:
    Texture *even;
    Texture *odd;
};

class NoiseTexture : public Texture
{
public:
    NoiseTexture() {}
    NoiseTexture(double sc) : scale(sc) {}

    virtual Color value(double u, double v, const Vec3 &p) const override
    {
        return Color(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turb(scale * p)));
    }

private:
    Perlin noise;
    double scale;
};

#endif