#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include <memory>

#include "hittable.h"
#include "texture.h"

class ConstantMedium : public Hittable
{
public:
    ConstantMedium(Hittable *b, double d, Material *a) : boundary(b), neg_inv_density(-1 / d), phase_function(a) {}

    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;
    virtual bool bounding_box(double time0, double time1, AABB &output_box) const override
    {
        return boundary->bounding_box(time0, time1, output_box);
    }

public:
    Hittable *boundary;
    double neg_inv_density;
    Material *phase_function;
};

bool ConstantMedium::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const
{
    HitRecord rec1, rec2;

    if (!boundary->hit(r, -infinity, infinity, rec1))
        return false;
    if (!boundary->hit(r, rec1.getT() + 0.0001, infinity, rec2))
        return false;

    if (rec1.getT() < t_min)
        rec1.setT(t_min);
    if (rec2.getT() > t_max)
        rec2.setT(t_max);
    if (rec1 >= rec2)
        return false;
    if (rec1.getT() < 0)
        rec1.setT(0);

    const auto ray_length = r.direction.length();
    const auto distance_inside_boundary = (rec2.getT() - rec1.getT()) * ray_length;
    const auto hit_distance = neg_inv_density * log(random_double());

    if (hit_distance > distance_inside_boundary)
        return false;

    double t = rec1.getT() + hit_distance / ray_length;

    rec.set(t, r.point_at_parameter(t), Vec3(1, 0, 0), phase_function, 0, 0);
    return true;
}

#endif