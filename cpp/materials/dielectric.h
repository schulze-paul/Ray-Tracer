/*
dielectric.h
============
Dielectric material.

A ray that hits a dielectric material can either be reflected or refracted.
The probability of reflection increases as the angle of incidence increases.
The probability of reflection also increases as the refractive index of the
material increases.
*/

#ifndef DIELECTRIC_H
#define DIELECTRIC_H

#include "materials.h"
#include "hit_record.h"

// Sellmeier equation
// https://en.wikipedia.org/wiki/Sellmeier_equation

const double B1 = 1.03961212;
const double B2 = 0.231792344;
const double B3 = 1.01046945;
const double C1 = 6.00069867 * 1e-3;
const double C2 = 2.00179144 * 1e-2;
const double C3 = 1.03560653 * 1e2;

double get_refractive_index(double wavelength)
{
    double wavelength_in_micro_meters = wavelength * 1e6;
    double l2 = wavelength_in_micro_meters * wavelength_in_micro_meters;
    return sqrt(1.0 + (B1 * l2) / (l2 - C1) + (B2 * l2) / (l2 - C2) + (B3 * l2) / (l2 - C3));
}	

/**
 * @brief      Class for dielectric.
 */
class Dielectric : public Material
{
private:
    double ref_idx;

public:
    Dielectric(double ri) : ref_idx(ri) {}

    /**
     * @brief      Scatter a ray from a dielectric material.
     * @param[in]  r_in         The incoming ray
     * @param[in]  hit_record   The hit record
     * @param[out] scatter_record   The scatter record
     */
    virtual bool scatter(const Ray &r_in, const HitRecord &hit_record, ScatterRecord &scatter_record) const override
    {
        scatter_record.is_specular = true;
        scatter_record.pdf = nullptr;
        scatter_record.attenuation = Color(1.0, 1.0, 1.0);

        double refractive_index = get_refractive_index(r_in.wavelength);
        double refraction_ratio = hit_record.is_front_face(r_in) ? (1.0 / refractive_index) : refractive_index;

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