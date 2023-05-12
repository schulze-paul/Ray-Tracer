/*
Rectangle.h
===========

The Rectangle class represents an axis-aligned rectangle.
The Box class is constructed from 6 axis-aligned rectangles.
*/

#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <memory>

#include "hittable.h"
#include "color.h"
#include "vec3.h"
#include "ray.h"
#include "hit_record.h"

/**
 * @brief      Class for rectangle along XY plane.
 */
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

/**
 * @brief      Class for rectangle along XZ plane.
 */
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

/**
 * @brief      Class for rectangle along YZ plane.
 */
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

/**
 * @brief     Check if a ray hits the rectangle.
 * @param[in] r Ray to check.
 * @param[in] t_min Minimum distance along the ray to check.
 * @param[in] t_max Maximum distance along the ray to check.
 * @param[out] rec Hit record.
 */
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

/**
 * @brief     Get the bounding box of the rectangle.
 * @param[in] time0 Start time.
 * @param[in] time1 End time.
 * @param[out] output_box Bounding box.
 * @return    True if bounding box is valid.
 */
bool XY_Rectangle::bounding_box(double time0, double time1, AABB &output_box) const
{
    output_box = AABB(Vec3(x0, y0, k - 0.0001), Vec3(x1, y1, k + 0.0001));
    return true;
}

/**
 * @brief     Check if a ray hits the rectangle.
 * @param[in] r Ray to check.
 * @param[in] t_min Minimum distance along the ray to check.
 * @param[in] t_max Maximum distance along the ray to check.
 * @param[out] rec Hit record.
 */
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

/**
 * @brief   Construct a bounding box for the rectangle.
 * @param[in] time0 Start time.
 * @param[in] time1 End time.
 * @param[out] output_box Bounding box.
 * @return    True if bounding box is valid.
*/
bool XZ_Rectangle::bounding_box(double time0, double time1, AABB &output_box) const
{
    output_box = AABB(Vec3(x0, k - 0.0001, z0), Vec3(x1, k + 0.0001, z1));
    return true;
}

/**
 * @brief     Check if a ray hits the rectangle.
 * @param[in] r Ray to check.
 * @param[in] t_min Minimum distance along the ray to check.
 * @param[in] t_max Maximum distance along the ray to check.
 * @param[out] rec Hit record.
 * @return    True if hit.
*/
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

/**
 * @brief   Construct a bounding box for the rectangle.
 * @param[in] time0 Start time.
 * @param[in] time1 End time.
 * @param[out] output_box Bounding box.
 * @return    True if bounding box is valid.
*/
bool YZ_Rectangle::bounding_box(double time0, double time1, AABB &output_box) const
{
    output_box = AABB(Vec3(k - 0.0001, y0, z0), Vec3(k + 0.0001, y1, z1));
    return true;
}

/**
 * @brief     Get a random point on the rectangle.
 * @param[in] origin Origin of the ray.
 * @return    Random point on the rectangle.
 */
Vec3 XY_Rectangle::random(const Vec3 &origin) const
{
    Vec3 random_point = Vec3(random_double(x0, x1), random_double(y0,y1), k);
    return random_point - origin;
}

/**
 * @brief     Get a random point on the rectangle.
 * @param[in] origin Origin of the ray.
 * @return    Random point on the rectangle.
 */
Vec3 XZ_Rectangle::random(const Vec3 &origin) const
{
    Vec3 random_point = Vec3(random_double(x0, x1), k, random_double(z0, z1));
    return random_point - origin;
}

/**
 * @brief     Get a random point on the rectangle.
 * @param[in] origin Origin of the ray.
 * @return    Random point on the rectangle.
 */
Vec3 YZ_Rectangle::random(const Vec3 &origin) const
{
    Vec3 random_point = Vec3(k, random_double(y0, y1), random_double(z0, z1));
    return random_point - origin;
}

/**
 * @brief     Get the probability density function value for a given ray.
 * @param[in] origin Origin of the ray.
 * @param[in] v Direction of the ray.
 * @param[in] time Time of the ray.
 * @return    Probability density function value.
 */
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

/**
 * @brief     Get the probability density function value for a given ray.
 * @param[in] origin Origin of the ray.
 * @param[in] v Direction of the ray.
 * @param[in] time Time of the ray.
 * @return    Probability density function value.
 */
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

/**
 * @brief     Get the probability density function value for a given ray.
 * @param[in] origin Origin of the ray.
 * @param[in] v Direction of the ray.
 * @param[in] time Time of the ray.
 * @return    Probability density function value.
 */
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

/**
 * @brief   Box class.
 */
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

/**
 * @brief     Box constructor.
 * @param[in] p0       First point.
 * @param[in] p1       Second point.
 * @param[in] material Material.
 */
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

/**
 * @brief      Check if the box is hit by a ray.
 * @param[in]  r     Ray.
 * @param[in]  t_min Minimum value of t.
 * @param[in]  t_max Maximum value of t.
 * @param[out] rec   Hit record.
 * @return     True if the box is hit, false otherwise.
 */
bool Box::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const
{
    return sides.hit(r, t_min, t_max, rec);
    // set normal to outward normal
    // TODO
}

class Triangle : public Hittable
{
public:
    Triangle() {}
    Triangle(const Vec3 &p0, const Vec3 &p1, const Vec3 &p2, Material *material);
    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;
    virtual bool bounding_box(double time0, double time1, AABB &output_box) const override
    {
        Vec3 box_min = Vec3(std::min(p0.x(), std::min(p1.x(), p2.x())),
                            std::min(p0.y(), std::min(p1.y(), p2.y())),
                            std::min(p0.z(), std::min(p1.z(), p2.z())));
        Vec3 box_max = Vec3(std::max(p0.x(), std::max(p1.x(), p2.x())),
                            std::max(p0.y(), std::max(p1.y(), p2.y())),
                            std::max(p0.z(), std::max(p1.z(), p2.z())));
        output_box = AABB(box_min, box_max);
        return true;
    }
    std::string to_string() const { return "Triangle"; }

public:
    Vec3 p0, p1, p2;
    Material *m;
};

/**
 * @brief     Triangle constructor.
 * @param[in] p0       First point.
 * @param[in] p1       Second point.
 * @param[in] p2       Third point.
 * @param[in] material Material.
 */
Triangle::Triangle(const Vec3 &p0, const Vec3 &p1, const Vec3 &p2, Material *material)
{
    this->p0 = p0;
    this->p1 = p1;
    this->p2 = p2;
    Vec3 normal = unit_vector(cross(p1 - p0, p2 - p0));
    m = material;
}

/**
 * @brief      Check if the triangle is hit by a ray.
 * @param[in]  r     Ray.
 * @param[in]  t_min Minimum value of t.
 * @param[in]  t_max Maximum value of t.
 * @param[out] rec   Hit record.
 * @return     True if the triangle is hit, false otherwise.
 */
bool Triangle::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const
{
    Vec3 e1 = p1 - p0;
    Vec3 e2 = p2 - p0;
    Vec3 s = r.origin - p0;
    Vec3 s1 = cross(r.direction, e2);
    Vec3 s2 = cross(s, e1);
    double t = dot(s2, e2) / dot(s1, e1);
    double b1 = dot(s1, s) / dot(s1, e1);
    double b2 = dot(s2, r.direction) / dot(s1, e1);
    if (t < t_min || t > t_max || b1 < 0 || b2 < 0 || b1 + b2 > 1)
        return false;
    rec.set_t(t);
    rec.set_hit_point(r.point_at_parameter(t));
    rec.set_normal(unit_vector(cross(e1, e2)));
    rec.set_material(m);
    return true;
}

class TriangularPrism : public Hittable
{
public:
    TriangularPrism() {}
    TriangularPrism(const Vec3 &p0, const Vec3 &p1, const Vec3 &p2, const Vec3 &p3, const Vec3 &p4, const Vec3 &p5, Material *material);
    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;
    virtual bool bounding_box(double time0, double time1, AABB &output_box) const override;
    std::string to_string() const { return "TriangularPrism"; }
    bool is_inside(const Vec3 &p) const
    {
        Vec3 face_normal_1 = unit_vector(cross(p1 - p0, p2 - p0));
        Vec3 face_normal_2 = unit_vector(cross(p4 - p3, p5 - p3));

        Vec3 face_normal_3 = unit_vector(cross(p0 - p1, p4 - p1));
        Vec3 face_normal_4 = unit_vector(cross(p2 - p1, p5 - p1));
        Vec3 face_normal_5 = unit_vector(cross(p0 - p2, p3 - p2));

        Vec3 body_center = (p0 + p1 + p2 + p3 + p4 + p5) / 6.0;

        Vec3 face_center_1 = (p0 + p1 + p2) / 3.0;
        Vec3 face_center_2 = (p3 + p4 + p5) / 3.0;

        Vec3 face_center_3 = (p0 + p1 + p4) / 3.0;
        Vec3 face_center_4 = (p1 + p2 + p5) / 3.0;
        Vec3 face_center_5 = (p0 + p2 + p3) / 3.0;

        if (dot(p - body_center, face_normal_1) < 0)
            face_normal_1 = -face_normal_1;
        if (dot(p - body_center, face_normal_2) < 0)
            face_normal_2 = -face_normal_2;
        if (dot(p - body_center, face_normal_3) < 0)
            face_normal_3 = -face_normal_3;
        if (dot(p - body_center, face_normal_4) < 0)
            face_normal_4 = -face_normal_4;
        if (dot(p - body_center, face_normal_5) < 0)
            face_normal_5 = -face_normal_5;
        
        if (dot(p - face_center_1, face_normal_1) < 0)
            return false;
        if (dot(p - face_center_2, face_normal_2) < 0)  
            return false;
        if (dot(p - face_center_3, face_normal_3) < 0)
            return false;
        if (dot(p - face_center_4, face_normal_4) < 0)  
            return false;
        if (dot(p - face_center_5, face_normal_5) < 0)  
            return false;
        
        return true;
    }

public:
    Vec3 p0, p1, p2, p3, p4, p5;
    HittableList sides;
    Material *m;
};

TriangularPrism::TriangularPrism(const Vec3 &p0, const Vec3 &p1, const Vec3 &p2, const Vec3 &p3, const Vec3 &p4, const Vec3 &p5, Material *material)
{
    this->p0 = p0;
    this->p1 = p1;
    this->p2 = p2;
    this->p3 = p3;
    this->p4 = p4;
    this->p5 = p5;
    m = material;

    sides.add(std::make_shared<Triangle>(p0, p1, p2, m));
    sides.add(std::make_shared<Triangle>(p3, p4, p5, m));
    sides.add(std::make_shared<Triangle>(p0, p1, p4, m));
    sides.add(std::make_shared<Triangle>(p0, p3, p4, m));
    sides.add(std::make_shared<Triangle>(p1, p2, p5, m));
    sides.add(std::make_shared<Triangle>(p1, p4, p5, m));
    sides.add(std::make_shared<Triangle>(p2, p0, p3, m));
    sides.add(std::make_shared<Triangle>(p2, p3, p5, m));
}

bool TriangularPrism::bounding_box(double time0, double time1, AABB &output_box) const
{
    double x_min = std::min(p0.x(), std::min(p1.x(), std::min(p2.x(), std::min(p3.x(), std::min(p4.x(), p5.x())))));
    double y_min = std::min(p0.y(), std::min(p1.y(), std::min(p2.y(), std::min(p3.y(), std::min(p4.y(), p5.y())))));
    double z_min = std::min(p0.z(), std::min(p1.z(), std::min(p2.z(), std::min(p3.z(), std::min(p4.z(), p5.z())))));
    double x_max = std::max(p0.x(), std::max(p1.x(), std::max(p2.x(), std::max(p3.x(), std::max(p4.x(), p5.x())))));
    double y_max = std::max(p0.y(), std::max(p1.y(), std::max(p2.y(), std::max(p3.y(), std::max(p4.y(), p5.y())))));
    double z_max = std::max(p0.z(), std::max(p1.z(), std::max(p2.z(), std::max(p3.z(), std::max(p4.z(), p5.z())))));

    output_box = AABB(Vec3(x_min, y_min, z_min), Vec3(x_max, y_max, z_max));
    return true;
}

bool TriangularPrism::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const
{
    return sides.hit(r, t_min, t_max, rec);
    // set the correct normal direction
    Vec3 point_normal_offset = 0.001 * rec.get_normal() + rec.get_hit_point();
    if (is_inside(point_normal_offset))
    {
        rec.set_normal(-rec.get_normal());
    }

}

#endif
