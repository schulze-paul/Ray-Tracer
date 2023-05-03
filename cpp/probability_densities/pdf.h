#ifndef PDF_H
#define PDF_H

#include <vector>
#include <memory>

#include "vec3.h"
#include "hittable_list.h"
#include "hittable.h"
#include "materials.h"

#include "onb.h"


class PDF
{
public:
    virtual void generate(Vec3 &direction, double &value) const = 0;
};

inline Vec3 random_cosine_direction();

class CosinePDF : public PDF
{
private:
    ONB uvw;

public:
    CosinePDF(const Vec3 &w)
    {
        uvw.build_from_w(w);
    }

    double get_pdf_value(const Vec3 &direction) const
    {
        auto cosine = dot(unit_vector(direction), uvw.w());
        return (cosine <= 0) ? 0 : cosine / pi;
    }

    virtual void generate(Vec3 &direction, double &value) const override
    {
        direction = uvw.local(random_cosine_direction());
        value = this->get_pdf_value(direction);	
    }
};

class HittablePDF : public PDF
{
private:
    Vec3 o;
    std::shared_ptr<HittableList> hittables;
    double time;

public:
    HittablePDF(std::shared_ptr<HittableList> hittables, const Vec3 &o, double time) : hittables(hittables), o(o), time(time) {}

    virtual void generate(Vec3 &direction, double &value) const override
    {
        int hittableIndex = random_int(0, hittables->size() - 1);
        std::shared_ptr<Hittable> hittable = hittables->get(hittableIndex);
        direction = hittable->random(o);
        value = hittable->pdf_value(o, direction, time); 
    }
};

class MixturePDF : public PDF
{
private:
    std::shared_ptr<PDF> p[2];

public:
    MixturePDF(std::shared_ptr<PDF> p0, std::shared_ptr<PDF> p1)
    {
        p[0] = p0;
        p[1] = p1;
    }
    virtual void generate(Vec3 &direction, double &value) const override
    {
        if (random_double() < 0.5)
        {
            p[0]->generate(direction, value);
        }
        else
        {
            p[1]->generate(direction, value);
        }
    }
};

inline Vec3 random_cosine_direction()
{
    auto r1 = random_double();
    auto r2 = random_double();
    auto z = sqrt(1 - r2);

    auto phi = 2 * pi * r1;
    auto x = cos(phi) * sqrt(r2);
    auto y = sin(phi) * sqrt(r2);

    return Vec3(x, y, z);
}

class MetalPDF : public PDF
{
private:
    Vec3 o;
    Vec3 normal;
    double fuzz;

public:
    MetalPDF(const Vec3 &o, const Vec3 &normal, double fuzz) : o(o), normal(normal), fuzz(fuzz) {}

    virtual void generate(Vec3 &direction, double &value) const override
    {
        Vec3 reflected = reflect(unit_vector(o), normal);
        direction = reflected + fuzz * random_in_unit_sphere();
        if (dot(direction, normal) > 0)
        {
            value = dot(reflected, direction) / dot(reflected, reflected);
        }
        else
        {
            value = 0;
        }
    }
};

class DielectricPDF : public PDF
{
private:
    Vec3 o;
    Vec3 normal;
    double ref_idx;

public:
    DielectricPDF(const Vec3 &o, const Vec3 &normal, double ref_idx) : o(o), normal(normal), ref_idx(ref_idx) {}

    virtual void generate(Vec3 &direction, double &value) const override
    {
        double cosine = dot(unit_vector(o), normal);
        double reflect_prob = reflectance(cosine, ref_idx);
        double refract_prob = 1 - reflect_prob;
        if (random_double() < reflect_prob)
        {
            direction = reflect(unit_vector(o), normal);
            value = reflect_prob;
        }
        else
        {
            direction = refract(unit_vector(o), normal, ref_idx);
            value = refract_prob;
        }
    }
};


#endif // PDF_H