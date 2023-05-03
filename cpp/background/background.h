#ifndef BACKGROUND_H
#define BACKGROUND_H


#include "color.h"
#include "ray.h"
#include "vec3.h"

class Background {
public:
    virtual Color get_color(const Ray& ray) const = 0;
};

class SolidBackground : public Background {
private:
    Color color;
public:
    SolidBackground(const Color& color) : color(color) {}
    Color get_color(const Ray& ray) const override {
        return color;
    }
};

class GradientBackground : public Background {
private:
    Color color1;
    Color color2;
public:
    GradientBackground(const Color& color1, const Color& color2) : color1(color1), color2(color2) {}
    Color get_color(const Ray& ray) const override {
        Vec3 unit_direction = unit_vector(ray.direction);
        double t = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - t) * color1 + t * color2;
    }
};

#endif // BACKGROUND_H