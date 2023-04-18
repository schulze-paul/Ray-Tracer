#ifndef HITTABLE_H
#define HITTABLE_H

#include <memory>

#include "vec3.h"
#include "ray.h"
#include "hit_record.h"
#include "materials.h"
#include "aabb.h"

class Hittable
{
public:
    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const = 0;
    void setMaterial(std::shared_ptr<Material> m);
    std::shared_ptr<Material> getMaterial() const { return material; }
    virtual bool bounding_box(double t0, double t1, AABB &box) const = 0;

private:
    std::shared_ptr<Material> material;
};

void Hittable::setMaterial(std::shared_ptr<Material> m)
{
    material = m;
}


void get_sphere_uv(const Vec3 &p, double &u, double &v)
{
    double phi = atan2(p.z(), p.x());
    double theta = asin(p.y());
    u = 1 - (phi + M_PI) / (2 * M_PI);
    v = (theta + M_PI / 2) / M_PI;
}


#endif // HITTABLE_H