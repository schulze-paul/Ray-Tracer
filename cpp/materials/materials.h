#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>

#include "color.h"
#include "ray.h"
#include "texture.h"

class HitRecord;

class Material
{
public:
    virtual bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation, Ray &scattered, double &pdf, std::shared_ptr<HittableList>& lights) const 
    {
        return false;
    };
    virtual Color emitted(double u, double v, const Vec3 &p) const = 0;
    std::shared_ptr<Texture> getAlbedo() const
    {
        return albedo;
    }
    void setAlbedo(std::shared_ptr<Texture> a)
    {
        albedo = a;
    }
    virtual std::string toString() {
        return "Material";
    };
    virtual double scattering_pdf(const Ray &r_in, const HitRecord &rec, const Ray &scattered) const
    {
        return 0;
    }
    virtual bool isEmissive() const
    {
        return false;
    }
    virtual bool isLambertian() const
    {
        return false;
    }
    
private:
    std::shared_ptr<Texture> albedo;
};


inline Vec3 reflect(const Vec3 &v, const Vec3 &n)
{
    return v - 2 * dot(v, n) * n;
}


inline Vec3 refract(const Vec3 &uv, const Vec3 &normal, double refraction_ratio)
{
    auto cos_theta = fmin(dot(-uv, normal), 1.0);
    Vec3 r_out_perp = refraction_ratio * (uv + cos_theta * normal);
    Vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * normal;
    return r_out_perp + r_out_parallel;
}

#endif