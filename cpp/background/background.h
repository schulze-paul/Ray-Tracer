/*
background.h
============
This class is used to represent the background of the scene. It is used to
determine the color of the background when a ray does not hit anything.

*/

#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "color.h"
#include "ray.h"
#include "vec3.h"

/**
 * @brief      Class for background.
 * @details    This class is used to represent the background of the scene. It is used to
 *             determine the color of the background when a ray does not hit anything.
 */
class Background {
public:
    virtual Color get_color(const Ray& ray) const = 0;
};

/**
 * @brief      Class for solid background.
 * @details    The color of the background is constant.
 */
class SolidBackground : public Background {
private:
    Color color;
public:
    SolidBackground(const Color& color) : color(color) {}
    Color get_color(const Ray& ray) const override;
};

/**
 * @brief      Gets the color.
 * @param[in]  ray   The ray
 * @return     The color.
 */
Color SolidBackground::get_color(const Ray& ray) const
{
    return color;
}

/**
 * @brief      Class for gradient background.
 * @details    The gradient is defined by two colors.
 *             The color of the background is determined by the direction of the ray.
 *             The color of the background is a linear interpolation between the two colors.
 *             The interpolation is based on the y-coordinate of the direction of the ray.
 *             The y-coordinate is normalized to the interval [0, 1].
 */
class GradientBackground : public Background {
private:
    Color color1;
    Color color2;
public:
    GradientBackground(const Color& color1, const Color& color2) : color1(color1), color2(color2) {}
    Color get_color(const Ray& ray) const override;
};

/**
 * @brief      Gets the color.
 * @param[in]  ray   The ray
 * @return     The color.
 */
Color GradientBackground::get_color(const Ray& ray) const
{
    Vec3 unit_direction = unit_vector(ray.direction);
    double t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color1 + t*color2;
}

#endif // BACKGROUND_H