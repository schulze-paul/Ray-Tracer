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
    std::shared_ptr<HittableList> lights;

public:
    Lambertian(const Color &a)
    {
        this->albedo = std::make_shared<SolidColor>(a);
        this->lights = std::make_shared<HittableList>();
    };
    Lambertian(const Color &a, std::shared_ptr<HittableList> lights)
    {
        this->albedo = std::make_shared<SolidColor>(a);
        this->lights = lights;
    };

    Lambertian(Texture *a, std::shared_ptr<HittableList> lights) : albedo(a), lights(lights) {}
    Lambertian(Texture *a) : albedo(a) {}
    virtual bool scatter(const Ray &r_in, const HitRecord &rec, Color &albedo, Ray &scattered, double &pdf) const override
    {
        auto light_pdf = std::make_shared<HittablePDF>(this->lights, rec.get_hit_point(), r_in.get_time());
        auto cosine_pdf = std::make_shared<CosinePDF>(rec.get_normal());
        MixturePDF mixture_pdf = MixturePDF(light_pdf, cosine_pdf);

        Vec3 direction;
        mixture_pdf.generate(direction, pdf);
    
        scattered = Ray(rec.get_hit_point(), unit_vector(direction), r_in.get_time());
        albedo = this->albedo->value(rec.u, rec.v, rec.get_hit_point());
        return true;
    }
    Color emitted(double u, double v, const Vec3 &p) const override
    {
        return Color(0, 0, 0);
    }
    std::string to_string() {
        return "Lambertian";
    }
    virtual bool is_lambertian() const override
    {
        return true;
    }
    void set_lights(std::shared_ptr<HittableList> lights) {
        this->lights = lights;
    }

};


#endif // LAMBERTIAN_H