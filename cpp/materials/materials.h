/*
materials.h
===========
Contains the abstract class for materials.
*/

#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>

#include "color.h"
#include "ray.h"
#include "texture.h"
#include "pdf.h"
#include "scatter_record.h"

class HitRecord;

class Material
{
public:
    virtual bool scatter(const Ray &r_in, const HitRecord &hit_record, ScatterRecord &scatter_record) const 
    {
        return false;
    };
    virtual Color emitted(double u, double v, const Vec3 &p) const = 0;
    std::shared_ptr<Texture> get_albedo() const
    {
        return albedo;
    }
    void set_albedo(std::shared_ptr<Texture> a)
    {
        albedo = a;
    }
    virtual std::string to_string() {
        return "Material";
    };
    virtual double scattering_pdf(const Ray &r_in, const HitRecord &rec, const Ray &scattered) const
    {
        return 0;
    }
    virtual bool is_emissive() const
    {
        return false;
    }
    virtual bool is_lambertian() const
    {
        return false;
    }
    
private:
    std::shared_ptr<Texture> albedo;
};

// Utility Functions

/**
 * @brief      Reflects an incoming direction against a surface with the given
 *             normal.
 * @param[in]  v     Incoming direction.
 * @param[in]  n     Surface normal.
 * @return     Reflected direction.
*/
inline Vec3 reflect(const Vec3 &v, const Vec3 &n)
{
    return v - 2 * dot(v, n) * n;
}

/**
 * @brief      Refracts an incoming direction against a surface with the given
 *            normal.
 * @param[in]  uv                Incoming direction.
 * @param[in]  normal            Surface normal.
 * @param[in]  refraction_ratio  The refraction ratio
 * @return     Refracted direction.
*/
inline Vec3 refract(const Vec3 &uv, const Vec3 &normal, double refraction_ratio)
{
    auto cos_theta = fmin(dot(-uv, normal), 1.0);
    Vec3 r_out_perp = refraction_ratio * (uv + cos_theta * normal);
    Vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * normal;
    return r_out_perp + r_out_parallel;
}

/**
 * @brief      Calculates the reflectance of a surface based on the cosine of
 *             the angle between the surface normal and the incoming ray.
 * @param[in]  cosine  The cosine of the angle between the surface normal and
 *                     the incoming ray.
 * @param[in]  ref_idx The refraction index.
 * @return     The reflectance.
 */
double reflectance(double cosine, double ref_idx)
{
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

#endif