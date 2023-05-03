#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "materials.h"
#include "solid_color.h"
#include "vec3.h"
#include "ray.h"
#include "hit_record.h"
#include "scatter_record.h"
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
    virtual bool scatter(const Ray &r_in, const HitRecord &hit_record, ScatterRecord &scatter_record) const override
    {
        auto light_pdf = std::make_shared<HittablePDF>(this->lights, hit_record.get_hit_point(), r_in.get_time());
        auto cosine_pdf = std::make_shared<CosinePDF>(hit_record.get_normal());
        auto mixture_pdf = std::make_shared<MixturePDF>(light_pdf, cosine_pdf);
        scatter_record.is_specular = false;
        scatter_record.attenuation = this->albedo->value(hit_record.u, hit_record.v, hit_record.get_hit_point());
        scatter_record.pdf = mixture_pdf;
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
    virtual double scattering_pdf(const Ray &r_in, const HitRecord &rec, const Ray &scattered) const override
    {
        double cosine = dot(rec.get_normal(), unit_vector(scattered.direction));
        return cosine < 0 ? 0 : cosine / pi;
    }

};


#endif // LAMBERTIAN_H