/*
ray.h
=====
Represents a ray in 3D space, defined by an origin, a direction and a time.
*/

#ifndef RAY_H
#define RAY_H

#include "vec3.h"

/**
 * @brief      Class for ray.
 */
class Ray
{
public:
    Vec3 origin;
    Vec3 direction;
    double time;
    double wavelength;

    Ray() {}
    Ray(const Vec3 &origin, const Vec3 &direction, double time) : origin(origin), direction(direction), time(time), wavelength(550){}
    Ray(const Vec3 &origin, const Vec3 &direction, double time, double wavelength) : origin(origin), direction(direction), time(time), wavelength(wavelength){}

    double get_time() const { return time; }
    Vec3 point_at_parameter(double t) const
    {
        return origin + t * direction;
    }
};

#endif