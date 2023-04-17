#ifndef MATERIAL_H
#define MATERIAL_H

#include "color.h"
#include "ray.h"

class HitRecord;

class Material
{
public:
    virtual bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation, Ray &scattered) const = 0;
};

inline Vec3 reflect(const Vec3 &v, const Vec3 &n)
{
    return v - 2 * dot(v, n) * n;
}

inline Vec3 refract(const Vec3 &uv, const Vec3 &n, double etai_over_etat)
{
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    Vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

#endif