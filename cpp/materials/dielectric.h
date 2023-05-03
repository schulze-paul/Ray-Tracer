#ifndef DIELECTRIC_H
#define DIELECTRIC_H

#include "materials.h"
#include "hit_record.h"
#include "scatter_record.h"


class Dielectric : public Material
{
private:
    double ref_idx;

public:
    Dielectric(double ri) : ref_idx(ri) {}
    virtual bool scatter(const Ray &r_in, const HitRecord &hit_record, ScatterRecord &scatter_record) const override
    {
        scatter_record.is_specular = true;
        scatter_record.pdf = nullptr;
        scatter_record.attenuation = Color(1.0, 1.0, 1.0);
        double refraction_ratio = hit_record.is_front_face(r_in) ? (1.0 / ref_idx) : ref_idx;

        Vec3 unit_direction = unit_vector(r_in.direction);
        Vec3 unit_normal = hit_record.is_front_face(r_in) ? hit_record.get_normal() : -hit_record.get_normal();
        double cos_theta = fmin(dot(-unit_direction, unit_normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        Vec3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
        {
            direction = reflect(unit_direction, unit_normal);
        }
        else
        {
            direction = refract(unit_direction, unit_normal, refraction_ratio);
        }

        scatter_record.specular_ray = Ray(hit_record.get_hit_point(), direction, r_in.time);
        return true;
    }
    Color emitted(double u, double v, const Vec3 &p) const override
    {
        return Color(0, 0, 0);
    }
    std::string toString() {
        return "Dielectric";
    }

};


#endif // DIELECTRIC_H