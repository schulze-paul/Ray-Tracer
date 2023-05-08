/*
Moving Sphere
=============
A sphere that moves over time.
*/

#ifndef MOVING_SPHERE_H
#define MOVING_SPHERE_H

#include "hittable.h"

/**
 * @brief      Class for moving sphere.
 */
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
        this->material = material;
    }

    Vec3 get_center(double time) const
    {
        return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
    }
    double get_radius() const { return radius; }

    /**
     * @brief      Determines if ray hits the sphere.
     * @param      r      The ray
     * @param[in]  t_min  The minimum t
     * @param[in]  t_max  The maximum t
     * @param[out] rec    The hit record
     * @return     True if hit, False otherwise.
     */
    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override
    {
        Vec3 center = get_center(r.time);
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
            Vec3 normal = (r.point_at_parameter(hit_at_t) - center) / radius;
            double u, v;
            get_sphere_uv(normal, u, v);
        rec.    set(hit_at_t, normal, r.point_at_parameter(hit_at_t), material, u, v);
            
            return true;
        }
        return false;
    }

    /**
     * @brief      Constructs the bounding box.
     * @param[in]  t0   The start time
     * @param[in]  t1   The end time
     * @param[out] box  The bounding box
    */
    virtual bool bounding_box(double t0, double t1, AABB &box) const override
    {
        AABB box0(get_center(t0) - Vec3(radius, radius, radius), getCenter(t0) + Vec3(radius, radius, radius));
        AABB box1(get_center(t1) - Vec3(radius, radius, radius), getCenter(t1) + Vec3(radius, radius, radius));
        box = surrounding_box(box0, box1);
        return true;
    }

private:
    Vec3 center0, center1;
    double time0, time1;
    double radius;
    Material *material;
};

#endif // MOVING_SPHERE_H