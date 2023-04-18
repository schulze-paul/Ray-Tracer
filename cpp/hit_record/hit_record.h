#ifndef HIT_RECORD_H
#define HIT_RECORD_H

#include <memory>

#include "ray.h"
#include "materials.h"

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
    HitRecord(double t, const Vec3 &normal, const Vec3 &hitPoint, Material *material, double u, double v)
    {
        this->hit = true;
        this->t = t;
        this->normal = normal;
        this->hitPoint = hitPoint;
        this->material = material;
        this->u = u;
        this->v = v;
    }
    void set(double t, const Vec3 &normal, const Vec3 &hitPoint, Material *material, double u, double v)
    {
        this->hit = true;
        this->t = t;
        this->normal = normal;
        this->hitPoint = hitPoint;
        this->material = material;
        this->u = u;
        this->v = v;
    }
    void setHit(bool hit) { this->hit = hit; }
    bool getHit() const { return hit; }

    void setT(double t) { this->t = t; }
    double getT() const { return t; }

    void setNormal(const Vec3 &normal) { this->normal = normal; }
    Vec3 getNormal() const { return normal; }

    void setHitPoint(const Vec3 &hitPoint) { this->hitPoint = hitPoint; }
    Vec3 getHitPoint() const { return hitPoint; }

    void setMaterial(Material *material) { this->material = material; }
    Material *getMaterial() const { return material; }

    double getU() const { return u; }
    double getV() const { return v; }

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

    bool isFrontFace(const Ray &r) const
    {
        return dot(r.direction, normal) < 0;
    }

private:
    bool hit;
    double t;
    Vec3 normal;
    Vec3 hitPoint;
    Ray ray;
    Material *material;
};

#endif // HIT_RECORD_H