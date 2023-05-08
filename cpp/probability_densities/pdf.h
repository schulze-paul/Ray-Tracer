/*
probability_densities/pdf.h
===========================
Probability density functions for use in Monte Carlo integration
of the rendering equation.
*/

#ifndef PDF_H
#define PDF_H

#include <vector>
#include <memory>

#include "vec3.h"
#include "hittable_list.h"
#include "hittable.h"

#include "onb.h"

/**
 * @brief      Base class for probability density function.
 */
class PDF
{
public:
    virtual void generate(Vec3 &direction, double &value) const = 0;
};

inline Vec3 random_cosine_direction();

/**
 * @brief      Class for cosine pdf.
 */
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

/**
 * @brief      Class for hittable pdf.
 */
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

/**
 * @brief      Class for mixture pdf.
 *             Generates a direction from one of two pdfs with equal probability.
 */
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

/**
 * @brief     Generates a random direction on a hemisphere.
*/
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

#endif // PDF_H