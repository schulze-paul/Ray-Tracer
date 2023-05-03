#ifndef TRANSLATIONS_H
#define TRANSLATIONS_H

#include <memory>

#include "hittable.h"

class Translate : public Hittable
{
public:
    Translate(std::shared_ptr<Hittable> hittable, const Vec3 &displacement) : hittable(hittable), offset(displacement) {}

    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;
    virtual bool bounding_box(double time0, double time1, AABB &output_box) const override;

public:
    std::shared_ptr<Hittable> hittable;
    Vec3 offset;
};

bool Translate::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const
{
    Ray moved_r(r.origin - offset, r.direction, r.get_time());
    if (!hittable->hit(moved_r, t_min, t_max, rec))
        return false;

    rec.setHitPoint(rec.getHitPoint() + offset);
    rec.setNormal(rec.getNormal());

    return true;
}

bool Translate::bounding_box(double time0, double time1, AABB &output_box) const
{
    if (!hittable->bounding_box(time0, time1, output_box))
        return false;

    output_box = AABB(output_box.getMin() + offset, output_box.getMax() + offset);
    return true;
}

#endif