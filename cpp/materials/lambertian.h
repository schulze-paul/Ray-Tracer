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
        Vec3 normal = rec.isFrontFace(r_in) ? rec.getNormal() : -rec.getNormal();
        Vec3 scatter_direction = normal + random_unit_vector();
        scattered = Ray(rec.getHitPoint(), scatter_direction, r_in.get_time());
        attenuation = albedo->value(rec.u, rec.v, rec.getHitPoint());
        return true;
    }
    Color emitted(double u, double v, const Vec3 &p) const override
    {
        return Color(0, 0, 0);
    }
};


#endif // LAMBERTIAN_H