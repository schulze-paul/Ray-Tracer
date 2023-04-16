#ifndef AABB_H
#define AABB_H

#include "ray.h"
#include "vec3.h"

class AABB
{
public:
    AABB() {}
    AABB(const Vec3 &a, const Vec3 &b)
    {
        minimum = a;
        maximum = b;
    }

    Vec3 getMin() const { return minimum; }
    Vec3 getMax() const { return maximum; }

    bool hit(const Ray &r, double t_min, double t_max) const;

    Vec3 minimum;
    Vec3 maximum;
};

inline bool AABB::hit(const Ray &r, double t_min, double t_max) const
{
    for (int a = 0; a < 3; a++)
    {
        double invD = 1.0 / r.direction[a];
        double t0 = (minimum[a] - r.origin[a]) * invD;
        double t1 = (maximum[a] - r.origin[a]) * invD;
        if (invD < 0.0)
        {
            std::swap(t0, t1);
        }
        t_min = t0 > t_min ? t0 : t_min;
        t_max = t1 < t_max ? t1 : t_max;
        if (t_max <= t_min)
        {
            return false;
        }
    }
    return true;
}

AABB surrounding_box(AABB box0, AABB box1)
{
    Vec3 small(fmin(box0.minimum.x(), box1.minimum.x()),
               fmin(box0.minimum.y(), box1.minimum.y()),
               fmin(box0.minimum.z(), box1.minimum.z()));
    Vec3 big(fmax(box0.maximum.x(), box1.maximum.x()),
             fmax(box0.maximum.y(), box1.maximum.y()),
             fmax(box0.maximum.z(), box1.maximum.z()));
    return AABB(small, big);
}

#endif