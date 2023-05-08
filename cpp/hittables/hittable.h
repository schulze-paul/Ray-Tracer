/*
Hittable
========
The Hittable class is an abstract class that represents an object that can be hit by a ray.
*/

#ifndef HITTABLE_H
#define HITTABLE_H

#include <memory>

#include "vec3.h"
#include "ray.h"
#include "aabb.h"


class Material;
class HitRecord;

/**
 * @brief      Class for hittable.
 * @details    The Hittable class is an abstract class that represents an object that can be hit by a ray.
 */
class Hittable
{
public:
    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const = 0;
    void set_material(Material *m);
    Material* get_material() const { return material; }
    virtual bool bounding_box(double t0, double t1, AABB &box) const = 0;
    
    virtual std::string to_string()
    {
        return "Hittable";
    };
    virtual double pdf_value(const Vec3 &o, const Vec3 &v, double time) const
    {
        return 0.0;
    }
    virtual Vec3 random(const Vec3 &o) const
    {
        return Vec3(1, 0, 0);
    }
private:
    Material *material;
};

/**
 * @brief      Sets the material.
 * @param      m     The material
 */
void Hittable::set_material(Material *m)
{
    material = m;
}

/**
 * @brief   Get the UV coordinates of a point on a sphere.
 * @param   p   The point
 * @param   u   The u coordinate
 * @param   v   The v coordinate
 */
void get_sphere_uv(const Vec3 &p, double &u, double &v)
{
    double phi = atan2(p.z(), p.x());
    double theta = asin(p.y());
    u = 1 - (phi + M_PI) / (2 * M_PI);
    v = (theta + M_PI / 2) / M_PI;
}


#endif // HITTABLE_H