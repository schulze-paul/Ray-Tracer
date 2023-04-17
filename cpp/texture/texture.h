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


#endif