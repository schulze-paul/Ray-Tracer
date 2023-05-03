#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

Vec3 random_to_sphere(double radius, double distance_squared);

class Sphere : public Hittable
{
public:
    Sphere() {}
    Sphere(Vec3 center, double radius, Material *material)
    {
        this->center = center;
        this->radius = radius;
        this->material = material;
    }

    Vec3 get_center() const { return center; }
    double get_radius() const { return radius; }

    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;
    Vec3 get_normal(const Vec3 &point) const { return (point - center) / radius; }
    virtual bool bounding_box(double t0, double t1, AABB &box) const override
    {
        box = AABB(center - Vec3(radius, radius, radius), center + Vec3(radius, radius, radius));
        return true;
    }
    std::string to_string() const
    {
        std::string ss = "";
        ss +="Sphere: \n";
        ss += "center: " + center.to_string() + "\n";
        ss += "radius: " + std::to_string(radius) + "\n";
        return ss;
    }
    void set_material(Material *m) { material = m; }
    double pdf_value(const Vec3& o, const Vec3& v, double time) const override {
        HitRecord rec;
        if (this->hit(Ray(o, v, time), 0.001, infinity, rec)) {
            auto cos_theta_max = sqrt(1 - radius*radius/(center-o).length_squared());
            auto solid_angle = 2*pi*(1-cos_theta_max);
            return 1 / solid_angle;
        }
        else {
            return 0;
        }
    }
    Vec3 random(const Vec3& o) const override {
        Vec3 direction = center - o;
        auto distance_squared = direction.length_squared();
        ONB uvw;
        uvw.build_from_w(direction);
        return uvw.local(random_to_sphere(radius, distance_squared));
    }
    
private:
    Vec3 center;
    double radius;
    Material *material;
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
        if (material == nullptr) {
            std::cerr << "material is null" << std::endl;
        }

        // sphere in range, compute hit
        double u, v;
        get_sphere_uv((rec.get_hit_point() - center) / radius, u, v);
        rec.set(hit_at_t, get_normal(r.point_at_parameter(hit_at_t)), r.point_at_parameter(hit_at_t), material, u, v);
        

        return true;
    }
    return false;
}

Vec3 random_to_sphere(double radius, double distance_squared) {
    auto r1 = random_double();
    auto r2 = random_double();
    auto z = 1 + r2*(sqrt(1-radius*radius/distance_squared) - 1);
    auto phi = 2*pi*r1;
    auto x = cos(phi)*sqrt(1-z*z);
    auto y = sin(phi)*sqrt(1-z*z);
    return Vec3(x, y, z);
}

#endif // SPHERE_H