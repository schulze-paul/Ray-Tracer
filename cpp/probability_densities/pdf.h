#ifndef PDF_H
#define PDF_H

#include <vector>
#include "vec3.h"
#include "dielectric.h"

class PDF
{
public:
    virtual void generate(std::vector<Vec3> &directions, std::vector<double> &values) const = 0;
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

    virtual void generate(std::vector<Vec3> &directions, std::vector<double> &values) const override
    {
        Vec3 direction = uvw.local(random_cosine_direction());
        directions.push_back(direction);
        double pdf_value = this->get_pdf_value(direction);
        values.push_back(pdf_value);	
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

    virtual void generate(std::vector<Vec3> &directions, std::vector<double> &values) const override
    {
        int hittableIndex = random_int(0, hittables->size() - 1);
        std::shared_ptr<Hittable> hittable = hittables->get(hittableIndex);
        Vec3 direction = hittable->random(o);
        directions.push_back(direction);
        values.push_back(hittable->pdf_value(o, direction, time)); 
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
    virtual void generate(std::vector<Vec3> &directions, std::vector<double> &values) const override
    {
        p[0]->generate(directions, values);
        p[1]->generate(directions, values);
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

    virtual void generate(std::vector<Vec3> &directions, std::vector<double> &values) const override
    {
        Vec3 reflected = reflect(unit_vector(o), normal);
        Vec3 direction = reflected + fuzz * random_in_unit_sphere();
        if (dot(direction, normal) > 0)
        {
            directions.push_back(direction);
            values.push_back(1);
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

    virtual void generate(std::vector<Vec3> &directions, std::vector<double> &values) const override
    {
        double cosine = dot(unit_vector(o), normal);
        double reflect_prob = reflectance(cosine, ref_idx);
        double refract_prob = 1 - reflect_prob;
        Vec3 reflected = reflect(unit_vector(o), normal);
        directions.push_back(reflected);
        values.push_back(1);
        Vec3 refracted = refract(unit_vector(o), normal, ref_idx);
        directions.push_back(refracted);
        values.push_back(1);
    }
};


#endif // PDF_H