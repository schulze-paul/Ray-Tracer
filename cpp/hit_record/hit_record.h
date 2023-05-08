/*
Hit Record
==========
This class is used to store information about a ray-object intersection.
*/

#ifndef HIT_RECORD_H
#define HIT_RECORD_H

#include <memory>

#include "ray.h"

class Material;

/**
 * @brief      This class is used to store information about a ray-object intersection.
 * @details    It stores the following information:
 *             - t: the distance along the ray to the intersection point
 *             - normal: the normal vector of the surface at the intersection point
 *             - hit_point: the intersection point
 *             - material: the material of the object that was hit
 *             - hit: a boolean value indicating whether the ray hit an object
 *             - u, v: the texture coordinates of the intersection point
 */
class HitRecord
{
public:
    double u;
    double v;

    HitRecord()
    {
        hit = false;
        t = 0;
    }
    HitRecord(double t, const Vec3 &normal, const Vec3 &hit_point, Material *material, double u, double v)
    {
        this->hit = true;
        this->t = t;
        this->normal = normal;
        this->hit_point = hit_point;
        this->material = material;
        this->u = u;
        this->v = v;
    }
    void set(double t, const Vec3 &normal, const Vec3 &hit_point, Material *material, double u, double v)
    {
        this->hit = true;
        this->t = t;
        this->normal = normal;
        this->hit_point = hit_point;
        this->material = material;
        this->u = u;
        this->v = v;
    }
    void set_hit(bool hit) { this->hit = hit; }
    bool get_hit() const { return hit; }

    void set_t(double t) { this->t = t; }
    double get_t() const { return t; }

    void set_normal(const Vec3 &normal) { this->normal = normal; }
    Vec3 get_normal() const { return normal; }

    void set_hit_point(const Vec3 &hit_point) { this->hit_point = hit_point; }
    Vec3 get_hit_point() const { return hit_point; }

    void set_material(Material *material) { this->material = material; }
    Material *get_material() const { return material; }

    double get_u() const { return u; }
    double get_v() const { return v; }

    bool operator<(const HitRecord &other) const
    {
        return t < other.t;
    }

    bool operator>(const HitRecord &other) const
    {
        return t > other.t;
    }

    bool operator<=(const HitRecord &other) const
    {
        return t <= other.t;
    }

    bool operator>=(const HitRecord &other) const
    {
        return t >= other.t;
    }

    bool operator==(const HitRecord &other) const
    {
        return t == other.t;
    }

    bool operator!=(const HitRecord &other) const
    {
        return t != other.t;
    }

    bool is_front_face(const Ray &r) const
    {
        return dot(r.direction, normal) < 0;
    }

private:
    bool hit;
    double t;
    Vec3 normal;
    Vec3 hit_point;
    Ray ray;
    Material *material;
};

#endif // HIT_RECORD_H