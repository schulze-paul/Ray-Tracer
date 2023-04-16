#ifndef HITTABLE_H
#define HITTABLE_H

#include "vec3.h"
#include "ray.h"
#include "hit_record.h"
#include "materials.h"
#include "aabb.h"

class Hittable
{
public:
    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const = 0;
    virtual Material *getMaterial() const { return material; }
    void setMaterial(Material *material) { this->material = material; }
    virtual bool bounding_box(double t0, double t1, AABB &box) const = 0;

private:
    Material *material;
};

#endif // HITTABLE_H