#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class Ray
{
public:
    Vec3 origin;
    Vec3 direction;
    double time;

    Ray() {}
    Ray(const Vec3 &origin, const Vec3 &direction, double time) : origin(origin), direction(direction), time(time) {}

    double get_time() const { return time; }
    Vec3 point_at_parameter(double t) const
    {
        return origin + t * direction;
    }
};

#endif