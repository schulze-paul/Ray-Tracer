#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class Sphere : public Hittable
{
public:
    Sphere() {}
    Sphere(Vec3 center, double radius, Material *material)
    {
        this->center = center;
        this->radius = radius;
        setMaterial(material);
    }

    Vec3 getCenter() const { return center; }
    double getRadius() const { return radius; }

    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;
    Vec3 get_normal(const Vec3 &point) const { return (point - center) / radius; }
    virtual bool bounding_box(double t0, double t1, AABB &box) const override
    {
        box = AABB(center - Vec3(radius, radius, radius), center + Vec3(radius, radius, radius));
        return true;
    }

private:
    Vec3 center;
    double radius;
};

bool Sphere::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const
{
    Vec3 oc = r.origin - center;              // origin to center
    double a = dot(r.direction, r.direction); // direction squared
    double b = 2.0 * dot(oc, r.direction);    // 2 * alignment of center direction and ray direction
    double c = dot(oc, oc) - radius * radius; // center distance squared - radius squared
    double discriminant = b * b - 4 * a * c;
    if (discriminant > 0)
    {
        // ray in direction of sphere
        double hit_at_t = (-b - sqrt(discriminant)) / (2.0 * a);
        if (!(hit_at_t < t_max && hit_at_t > t_min))
        {
            // not in range, try other hit
            hit_at_t = (-b + sqrt(discriminant)) / (2.0 * a);
            if (!(hit_at_t < t_max && hit_at_t > t_min))
            {
                // not in range, no hit
                return false;
            }
        }

        // sphere in range, compute hit
        rec.setT(hit_at_t);
        rec.setHit(true);
        rec.setMaterial(getMaterial());
        Vec3 normal = get_normal(r.point_at_parameter(hit_at_t));
        rec.setNormal(normal);
        rec.setHitPoint(r.point_at_parameter(hit_at_t));
        return true;
    }
    return false;
}

#endif // SPHERE_H