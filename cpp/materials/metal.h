/*
metal.h
=======
A material that reflects light in a specular manner.
A surface roughness parameter is used to control the amount of fuzziness.
*/

#ifndef METAL_H
#define METAL_H

#include "materials/materials.h"
#include "hit_record.h"
#include "vec3.h"

/**
 * @brief      Class for metal.
*/
class Metal : public Material
{
private:
    Color albedo;
    double fuzz;

public:
    Metal(const Color &a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}
    virtual bool scatter(const Ray &r_in, const HitRecord &rec, ScatterRecord &scatter_record) const override
    {
        Vec3 reflected = reflect(unit_vector(r_in.direction), rec.get_normal());
        scatter_record.specular_ray = Ray(rec.get_hit_point(), reflected + fuzz * random_in_unit_sphere(), r_in.get_time());
        scatter_record.attenuation = albedo;
        scatter_record.is_specular = true;
        scatter_record.pdf = nullptr;
        return true;
    }
    Color emitted(double u, double v, const Vec3 &p) const override
    {
        return Color(0, 0, 0);
    }
    std::string to_string() {
        return "Metal";
    }
};

#endif // METAL_H