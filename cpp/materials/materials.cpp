#ifndef MATERIALS_H
#define MATERIALS_H

#include <memory>

#include "ray.h"
#include "vec3.h"
#include "color.h"
#include "materials.h"
#include "hit_record.h"
#include "texture.h"

inline Vec3 reflect(const Vec3 &v, const Vec3 &n)
{
    return v - 2 * dot(v, n) * n;
}

inline Vec3 refract(const Vec3 &uv, const Vec3 &n, double etai_over_etat)
{
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    Vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

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

class Dielectric : public Material
{
private:
    double ref_idx;

public:
    Dielectric(double ri) : ref_idx(ri) {}
    virtual bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation, Ray &scattered) const override
    {
        attenuation = Color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.isFrontFace(r_in) ? (1.0 / ref_idx) : ref_idx;
        Vec3 unit_direction = unit_vector(r_in.direction);
        Vec3 normal = rec.isFrontFace(r_in) ? rec.getNormal() : -rec.getNormal();

        double cos_theta = fmin(dot(-unit_direction, normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        Vec3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
            direction = reflect(unit_direction, normal);
        else
            direction = refract(unit_direction, normal, refraction_ratio);

        scattered = Ray(rec.getHitPoint(), unit_vector(direction), r_in.time);
        return true;
    }

private:
    static double reflectance(double cosine, double ref_idx)
    {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

#endif // MATERIALS_H