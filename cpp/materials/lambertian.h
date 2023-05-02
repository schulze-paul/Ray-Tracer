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
    virtual bool scatter(const Ray &r_in, const HitRecord &rec, Color &albedo, Ray &scattered, double &pdf, std::shared_ptr<HittableList>& lights) const override
    {
        auto light_pdf = std::make_shared<HittablePDF>(lights, rec.getHitPoint(), r_in.get_time());
        auto cosine_pdf = std::make_shared<CosinePDF>(rec.getNormal());
        MixturePDF mixture_pdf = MixturePDF(light_pdf, cosine_pdf);

        Vec3 direction;
        mixture_pdf.generate(direction, pdf);
    
        scattered = Ray(rec.getHitPoint(), unit_vector(direction), r_in.get_time());
        albedo = this->albedo->value(rec.u, rec.v, rec.getHitPoint());
        return true;
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