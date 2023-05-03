#ifndef ROTATE_H
#define ROTATE_H

#include <cmath>
#include <memory>

#include "hittable.h"
#include "vec3.h"
#include "ray.h"
#include "aabb.h"


class RotateY : public Hittable
{
public:
    RotateY(std::shared_ptr<Hittable> hittable, double angle)
    {            
        this->hittable = hittable;
        auto radians = degrees_to_radians(angle);
        sin_theta = std::sin(radians);
        cos_theta = std::cos(radians);
        has_box = hittable->bounding_box(0, 1, bbox);

        Vec3 min(infinity, infinity, infinity);
        Vec3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int k = 0; k < 2; k++)
                {
                    auto x = i * bbox.getMax().x() + (1 - i) * bbox.getMin().x();
                    auto y = j * bbox.getMax().y() + (1 - j) * bbox.getMin().y();
                    auto z = k * bbox.getMax().z() + (1 - k) * bbox.getMin().z();

                    auto newx = cos_theta * x + sin_theta * z;
                    auto newz = -sin_theta * x + cos_theta * z;

                    Vec3 tester(newx, y, newz);

                    for (int c = 0; c < 3; c++)
                    {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = AABB(min, max);
    }

    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;
    virtual bool bounding_box(double time0, double time1, AABB &output_box) const override
    {
        output_box = bbox;
        return has_box;
    }

public:
    std::shared_ptr<Hittable> hittable;
    double sin_theta;
    double cos_theta;
    bool has_box;
    AABB bbox;
};


bool RotateY::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const
{
    auto origin = r.origin;
    auto direction = r.direction;

    origin[0] = cos_theta * r.origin[0] - sin_theta * r.origin[2];
    origin[2] = sin_theta * r.origin[0] + cos_theta * r.origin[2];

    direction[0] = cos_theta * r.direction[0] - sin_theta * r.direction[2];
    direction[2] = sin_theta * r.direction[0] + cos_theta * r.direction[2];

    Ray rotated_r(origin, direction, r.get_time());

    if (!hittable->hit(rotated_r, t_min, t_max, rec))
        return false;

    auto p = rec.get_hit_point();
    auto normal = rec.get_normal();

    p[0] = cos_theta * rec.get_hit_point()[0] + sin_theta * rec.get_hit_point()[2];
    p[2] = -sin_theta * rec.get_hit_point()[0] + cos_theta * rec.get_hit_point()[2];

    normal[0] = cos_theta * rec.get_normal()[0] + sin_theta * rec.get_normal()[2];
    normal[2] = -sin_theta * rec.get_normal()[0] + cos_theta * rec.get_normal()[2];

    rec.set_hit_point(p);
    rec.set_normal(normal);

    return true;
}

#endif