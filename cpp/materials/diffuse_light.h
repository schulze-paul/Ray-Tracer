#ifndef DIFFUSE_LIGHT_H
#define DIFFUSE_LIGHT_H

#include "materials.h"
#include "color.h"
#include "vec3.h"
#include "ray.h"
#include "hit_record.h"
#include "texture.h"


class DiffuseLight : public Material
{
private:
    std::shared_ptr<Texture> albedo;

public:
    DiffuseLight(const Color &a)
    {
        this->albedo = std::make_shared<SolidColor>(a);
    };

    DiffuseLight(Texture *a) : albedo(a) {}
    virtual bool scatter(const Ray &r_in, const HitRecord &rec, ScatterRecord &scatter_record) const override
    {
        return false;
    }
    Color emitted(double u, double v, const Vec3 &p) const override
    {
        return albedo->value(u, v, p);
    }
    std::string toString() {
        return "DiffuseLight";
    }
    virtual bool is_emissive() const override
    {
        return true;
    }
};


#endif