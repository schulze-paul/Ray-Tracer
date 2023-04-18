#ifndef METAL_H
#define METAL_H

#include "materials/materials.h"
#include "hit_record.h"
#include "vec3.h"

class Metal : public Material
{
private:
    Color albedo;
    double fuzz;

public:
    Metal(const Color &a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}
    virtual bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation, Ray &scattered) const override
    {
        Vec3 normal = rec.isFrontFace(r_in) ? rec.getNormal() : -rec.getNormal();
        Vec3 reflected = reflect(unit_vector(r_in.direction), normal);
        scattered = Ray(rec.getHitPoint(), reflected + fuzz * random_in_unit_sphere(), r_in.time);
        attenuation = albedo;
        return true;//(dot(scattered.direction, normal) > 0);
    }
    Color emitted(double u, double v, const Vec3 &p) const override
    {
        return Color(0, 0, 0);
    }
};

#endif // METAL_H