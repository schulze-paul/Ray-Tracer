/*
Translation.h
=============

Implements a translation of a hittable object.
*/

#ifndef TRANSLATIONS_H
#define TRANSLATIONS_H

#include <memory>

#include "hittable.h"

/**
 * @brief      Class for translate.
 */
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

/**
 * @brief      Determines if the ray hits the hittable object.
 * @param[in]  r      The ray
 * @param[in]  t_min  The minimum parameter
 * @param[in]  t_max  The maximum parameter
 * @param[out] rec    The hit record
 * @return     True if the ray hits the hittable object, False otherwise.
*/
bool Translate::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const
{
    Ray moved_r(r.origin - offset, r.direction, r.get_time());
    if (!hittable->hit(moved_r, t_min, t_max, rec))
        return false;

    rec.set_hit_point(rec.get_hit_point() + offset);
    rec.set_normal(rec.get_normal());

    return true;
}

/**
 * @brief      Determines if the ray hits the bounding box.
 * @param[in]  time0       The time 0
 * @param[in]  time1       The time 1
 * @param[out] output_box  The output box
*/
bool Translate::bounding_box(double time0, double time1, AABB &output_box) const
{
    if (!hittable->bounding_box(time0, time1, output_box))
        return false;

    output_box = AABB(output_box.get_min() + offset, output_box.get_max() + offset);
    return true;
}

#endif