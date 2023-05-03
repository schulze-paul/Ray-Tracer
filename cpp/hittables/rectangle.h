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
    XY_Rectangle(double _x0, double _x1, double _y0, double _y1, double _k, Material *m) : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), material(m) {}
    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;
    virtual bool bounding_box(double time0, double time1, AABB &output_box) const override;
    std::string to_string() const { return "XY_Rectangle"; }
    double pdf_value(const Vec3 &o, const Vec3 &v, double time) const override;
    Vec3 random(const Vec3 &o) const override;

public:
    double x0, x1, y0, y1, k;
    Material *material;
};

class XZ_Rectangle : public Hittable
{
public:
    XZ_Rectangle() {}
    XZ_Rectangle(double _x0, double _x1, double _z0, double _z1, double _k, Material *m) : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), material(m) {}
    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;
    virtual bool bounding_box(double time0, double time1, AABB &output_box) const override;
    std::string to_string() const { return "XZ_Rectangle"; }
    double pdf_value(const Vec3 &o, const Vec3 &v, double time) const override;
    Vec3 random(const Vec3 &o) const override;

public:
    double x0, x1, z0, z1, k;
    Material *material;
};

class YZ_Rectangle : public Hittable
{
public:
    YZ_Rectangle() {}
    YZ_Rectangle(double _y0, double _y1, double _z0, double _z1, double _k, Material *m) : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), material(m) {}
    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;
    virtual bool bounding_box(double time0, double time1, AABB &output_box) const override;
    std::string to_string() const { return "YZ_Rectangle"; }
    double pdf_value(const Vec3 &o, const Vec3 &v, double time) const override;
    Vec3 random(const Vec3 &o) const override;

public:
    double y0, y1, z0, z1, k;
    Material *material;
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

Vec3 XY_Rectangle::random(const Vec3 &origin) const
{
    Vec3 random_point = Vec3(random_double(x0, x1), random_double(y0,y1), k);
    return random_point - origin;
}

Vec3 XZ_Rectangle::random(const Vec3 &origin) const
{
    Vec3 random_point = Vec3(random_double(x0, x1), k, random_double(z0, z1));
    return random_point - origin;
}

Vec3 YZ_Rectangle::random(const Vec3 &origin) const
{
    Vec3 random_point = Vec3(k, random_double(y0, y1), random_double(z0, z1));
    return random_point - origin;
}

double XY_Rectangle::pdf_value(const Vec3 &origin, const Vec3 &v, double time) const
{
    HitRecord rec;
    if (this->hit(Ray(origin, v, time), 0.001, infinity, rec))
    {
        double area = (x1 - x0) * (y1 - y0);
        double distance_squared = rec.get_t() * rec.get_t() * v.length_squared();
        double cosine = fabs(dot(v, rec.get_normal()) / v.length());
        return distance_squared / (cosine * area);
    }
    else
        return 0;
}

double XZ_Rectangle::pdf_value(const Vec3 &origin, const Vec3 &v, double time) const
{
    HitRecord rec;
    if (this->hit(Ray(origin, v, time), 0.001, infinity, rec))
    {
        double area = (x1 - x0) * (z1 - z0);
        double distance_squared = rec.get_t() * rec.get_t() * v.length_squared();
        double cosine = fabs(dot(v, rec.get_normal()) / v.length());
        return distance_squared / (cosine * area);
    }
    else
        return 0;
}

double YZ_Rectangle::pdf_value(const Vec3 &origin, const Vec3 &v, double time) const
{
    HitRecord rec;
    if (this->hit(Ray(origin, v, time), 0.001, infinity, rec))
    {
        double area = (y1 - y0) * (z1 - z0);
        double distance_squared = rec.get_t() * rec.get_t() * v.length_squared();
        double cosine = fabs(dot(v, rec.get_normal()) / v.length());
        return distance_squared / (cosine * area);
    }
    else
        return 0;
}


class Box : public Hittable
{
public:
    Box() {}
    Box(const Vec3 &p0, const Vec3 &p1, Material *material);
    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;
    virtual bool bounding_box(double time0, double time1, AABB &output_box) const override
    {
        output_box = AABB(box_min, box_max);
        return true;
    }
    std::string to_string() const { return "Box"; }

public:
    Vec3 box_min, box_max;
    HittableList sides;
};

Box::Box(const Vec3 &p0, const Vec3 &p1, Material *material)
{
    box_min = p0;
    box_max = p1;
    double min_x = std::min(p0.x(), p1.x());
    double min_y = std::min(p0.y(), p1.y());
    double min_z = std::min(p0.z(), p1.z());
    double max_x = std::max(p0.x(), p1.x());
    double max_y = std::max(p0.y(), p1.y());
    double max_z = std::max(p0.z(), p1.z());
    sides.add(std::make_shared<XY_Rectangle>(min_x, max_x, min_y, max_y, max_z, material));
    sides.add(std::make_shared<XY_Rectangle>(min_x, max_x, min_y, max_y, min_z, material));
    sides.add(std::make_shared<XZ_Rectangle>(min_x, max_x, min_z, max_z, max_y, material));
    sides.add(std::make_shared<XZ_Rectangle>(min_x, max_x, min_z, max_z, min_y, material));
    sides.add(std::make_shared<YZ_Rectangle>(min_y, max_y, min_z, max_z, max_x, material));
    sides.add(std::make_shared<YZ_Rectangle>(min_y, max_y, min_z, max_z, min_x, material));
}

bool Box::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const
{
    return sides.hit(r, t_min, t_max, rec);
}

#endif