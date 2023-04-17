#ifndef DIELECTRIC_H
#define DIELECTRIC_H

#include "materials.h"
#include "hit_record.h"

class Dielectric : public Material
{
private:
    double ref_idx;

public:
    Dielectric(double ri) : ref_idx(ri) {}
    virtual bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation, Ray &scattered) const override
    {
        attenuation = Color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.isFrontFace(r_in) ? (1.0 / ref_idx) : ref_idx;
        Vec3 unit_direction = unit_vector(r_in.direction);
        Vec3 normal = rec.isFrontFace(r_in) ? rec.getNormal() : -rec.getNormal();

        double cos_theta = fmin(dot(-unit_direction, normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        Vec3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
            direction = reflect(unit_direction, normal);
        else
            direction = refract(unit_direction, normal, refraction_ratio);

        scattered = Ray(rec.getHitPoint(), unit_vector(direction), r_in.time);
        return true;
    }

private:
    static double reflectance(double cosine, double ref_idx)
    {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

#endif // DIELECTRIC_H