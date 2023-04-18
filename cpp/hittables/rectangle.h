#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <memory>

#include "hittable.h"
#include "color.h"
#include "vec3.h"
#include "ray.h"
#include "hit_record.h"


class XY_Rectangle : public Hittable
{
public:
    XY_Rectangle() {}
    XY_Rectangle(double _x0, double _x1, double _y0, double _y1, double _k, std::shared_ptr<Material> m) : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), material(m) {}
    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;
    virtual bool bounding_box(double time0, double time1, AABB &output_box) const override;
    std::string toString() const override { return "XY_Rectangle"; }

public:
    double x0, x1, y0, y1, k;
    std::shared_ptr<Material> material;
};

class XZ_Rectangle : public Hittable
{
public:
    XZ_Rectangle() {}
    XZ_Rectangle(double _x0, double _x1, double _z0, double _z1, double _k, std::shared_ptr<Material> m) : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), material(m) {}
    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;
    virtual bool bounding_box(double time0, double time1, AABB &output_box) const override;
    std::string toString() const override { return "XZ_Rectangle"; }

public:
    double x0, x1, z0, z1, k;
    std::shared_ptr<Material> material;
};

class YZ_Rectangle : public Hittable
{
public:
    YZ_Rectangle() {}
    YZ_Rectangle(double _y0, double _y1, double _z0, double _z1, double _k, std::shared_ptr<Material> m) : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), material(m) {}
    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;
    virtual bool bounding_box(double time0, double time1, AABB &output_box) const override;
    std::string toString() const override { return "YZ_Rectangle"; }

public:
    double y0, y1, z0, z1, k;
    std::shared_ptr<Material> material;
};

bool XY_Rectangle::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const
{
    double t = (k - r.origin.z()) / r.direction.z();
    if (t < t_min || t > t_max)
        return false;
    double x = r.origin.x() + t * r.direction.x();
    double y = r.origin.y() + t * r.direction.y();
    if (x < x0 || x > x1 || y < y0 || y > y1)
        return false;
    double u = (x - x0) / (x1 - x0);
    double v = (y - y0) / (y1 - y0);
    Vec3 normal = Vec3(0, 0, 1);
    rec.set(t, normal, r.point_at_parameter(t), material, u, v);
    return true;
}

bool XY_Rectangle::bounding_box(double time0, double time1, AABB &output_box) const
{
    output_box = AABB(Vec3(x0, y0, k - 0.0001), Vec3(x1, y1, k + 0.0001));
    return true;
}

bool XZ_Rectangle::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const
{
    double t = (k - r.origin.y()) / r.direction.y();
    if (t < t_min || t > t_max)
        return false;
    double x = r.origin.x() + t * r.direction.x();
    double z = r.origin.z() + t * r.direction.z();
    if (x < x0 || x > x1 || z < z0 || z > z1)
        return false;
    double u = (x - x0) / (x1 - x0);
    double v = (z - z0) / (z1 - z0);
    Vec3 normal = Vec3(0, 1, 0);
    rec.set(t, normal, r.point_at_parameter(t), material, u, v); 
    return true;
}

bool XZ_Rectangle::bounding_box(double time0, double time1, AABB &output_box) const
{
    output_box = AABB(Vec3(x0, k - 0.0001, z0), Vec3(x1, k + 0.0001, z1));
    return true;
}

bool YZ_Rectangle::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const
{
    double t = (k - r.origin.x()) / r.direction.x();
    if (t < t_min || t > t_max)
        return false;
    double y = r.origin.y() + t * r.direction.y();
    double z = r.origin.z() + t * r.direction.z();
    if (y < y0 || y > y1 || z < z0 || z > z1)
        return false;
    double u = (y - y0) / (y1 - y0);
    double v = (z - z0) / (z1 - z0);
    Vec3 normal = Vec3(1, 0, 0);
    rec.set(t, normal, r.point_at_parameter(t), material, u, v);
    return true;
}

bool YZ_Rectangle::bounding_box(double time0, double time1, AABB &output_box) const
{
    output_box = AABB(Vec3(k - 0.0001, y0, z0), Vec3(k + 0.0001, y1, z1));
    return true;
}

class Box : public Hittable
{
public:
    Box() {}
    Box(const Vec3 &p0, const Vec3 &p1, std::shared_ptr<Material> ptr);
    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;
    virtual bool bounding_box(double time0, double time1, AABB &output_box) const override
    {
        output_box = AABB(box_min, box_max);
        return true;
    }
    std::string toString() const override { return "Box"; }

public:
    Vec3 box_min, box_max;
    HittableList sides;
};

Box::Box(const Vec3 &p0, const Vec3 &p1, std::shared_ptr<Material> ptr)
{
    box_min = p0;
    box_max = p1;
    sides.add(std::make_shared<XY_Rectangle>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
    sides.add(std::make_shared<XY_Rectangle>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));
    sides.add(std::make_shared<XZ_Rectangle>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
    sides.add(std::make_shared<XZ_Rectangle>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));
    sides.add(std::make_shared<YZ_Rectangle>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
    sides.add(std::make_shared<YZ_Rectangle>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
}

bool Box::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const
{
    return sides.hit(r, t_min, t_max, rec);
}

#endif