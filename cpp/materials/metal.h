#ifndef METAL_H
#define METAL_H

#include "materials/materials.h"
#include "hit_record.h"

class Metal : public Material
{
private:
    Color albedo;
    double fuzz;

public:
    Metal(const Color &a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}
    virtual bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation, Ray &scattered) const override
    {
        Vec3 reflected = reflect(unit_vector(r_in.direction), rec.getNormal());
        scattered = Ray(rec.getHitPoint(), reflected + fuzz * random_in_unit_sphere(), r_in.time);
        attenuation = albedo;
        return (dot(scattered.direction, rec.getNormal()) > 0);
    }
};

#endif // METAL_H