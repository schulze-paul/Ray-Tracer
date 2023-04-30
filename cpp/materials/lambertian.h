#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "materials.h"
#include "solid_color.h"
#include "vec3.h"
#include "ray.h"
#include "hit_record.h"
#include "onb.h"
#include "pdf.h"

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
    virtual bool scatter(const Ray &r_in, const HitRecord &rec, Color &albedo, Ray &scattered, double &pdf) const override
    {
        ONB uvw;
        uvw.build_from_w(rec.getNormal());
        auto direction = uvw.local(random_cosine_direction());
        scattered = Ray(rec.getHitPoint(), unit_vector(direction), r_in.get_time());
        albedo = this->albedo->value(rec.u, rec.v, rec.getHitPoint());
        pdf = dot(uvw.w(), scattered.direction) / pi;
        return true;
    }
    double scattering_pdf(const Ray &r_in, const HitRecord &hit_record, const Ray& scattered) const override
    {
        auto cosine = dot(hit_record.getNormal(), unit_vector(scattered.direction));
        return cosine < 0 ? 0 : cosine/pi;
    }
    Color emitted(double u, double v, const Vec3 &p) const override
    {
        return Color(0, 0, 0);
    }
    std::string toString() {
        return "Lambertian";
    }
    virtual bool isLambertian() const override
    {
        return true;
    }
};


#endif // LAMBERTIAN_H