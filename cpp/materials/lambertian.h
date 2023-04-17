#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "materials.h"
#include "solid_color.h"
#include "vec3.h"
#include "ray.h"
#include "hit_record.h"

class Lambertian : public Material
{
private:
    std::shared_ptr<Texture> albedo;

public:
    Lambertian(const Color &a)
    {
        this->albedo = std::make_shared<SolidColor>(a);
    };

    Lambertian(Texture *a) : albedo(a) {}
    virtual bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation, Ray &scattered) const override
    {
        Vec3 scatter_direction = rec.getNormal() + random_unit_vector();
        scattered = Ray(rec.getHitPoint(), scatter_direction, r_in.time);
        attenuation = albedo->value(rec.getU(), rec.getV(), rec.getHitPoint());
        return true;
    }
};

#endif // LAMBERTIAN_H