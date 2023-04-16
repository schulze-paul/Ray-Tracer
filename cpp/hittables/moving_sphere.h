#ifndef MOVING_SPHERE_H
#define MOVING_SPHERE_H

#include "hittable.h"

class MovingSphere : public Hittable
{
public:
    MovingSphere() {}
    MovingSphere(Vec3 center0, Vec3 center1, double time0, double time1, double radius, Material *material)
    {
        this->center0 = center0;
        this->center1 = center1;
        this->time0 = time0;
        this->time1 = time1;
        this->radius = radius;
        setMaterial(material);
    }

    Vec3 getCenter(double time) const
    {
        return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
    }
    double getRadius() const { return radius; }

    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override
    {
        Vec3 center = getCenter(r.time);
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
            Vec3 normal = (r.point_at_parameter(hit_at_t) - center) / radius;
            rec.setNormal(normal);
            rec.setHitPoint(r.point_at_parameter(hit_at_t));
            get_sphere_uv(normal, rec.u, rec.v);
            return true;
        }
        return false;
    }

    virtual bool bounding_box(double t0, double t1, AABB &box) const override
    {
        AABB box0(getCenter(t0) - Vec3(radius, radius, radius), getCenter(t0) + Vec3(radius, radius, radius));
        AABB box1(getCenter(t1) - Vec3(radius, radius, radius), getCenter(t1) + Vec3(radius, radius, radius));
        box = surrounding_box(box0, box1);
        return true;
    }

private:
    Vec3 center0, center1;
    double time0, time1;
    double radius;

    static void get_sphere_uv(const Vec3 &p, double &u, double &v)
    {
        double phi = atan2(p.z(), p.x());
        double theta = asin(p.y());
        u = 1 - (phi + pi) / (2 * pi);
        v = (theta + pi / 2) / pi;
    }
};

#endif // MOVING_SPHERE_H