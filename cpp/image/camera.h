#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "ray.h"
#include "color.h"

class Camera
{
public:
    Camera(){};
    Camera(double vfov, double aspect_ratio, double aperture, double focus_dist, Vec3 look_from, Vec3 look_at, double time0 = 0, double time1 = 0)
    {
        setUp(vfov, aspect_ratio, aperture, focus_dist, look_from, look_at, time0, time1);
    }

    void setUp(double vfov, double aspect_ratio, double aperture, double focus_dist, Vec3 look_from, Vec3 look_at, double time0 = 0, double time1 = 0)
    {
        double theta = degrees_to_radians(vfov);
        double h = tan(theta / 2);
        this->viewport_height = 2.0 * h;
        this->viewport_width = aspect_ratio * viewport_height;

        this->w = unit_vector(look_from - look_at);
        this->u = unit_vector(cross(Vec3(0, 1, 0), w));
        this->v = cross(w, u);

        this->origin = look_from;
        this->horizontal = focus_dist * viewport_width * u;
        this->vertical = focus_dist * viewport_height * v;
        this->lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

        this->lens_radius = aperture / 2;
        this->time0 = time0;
        this->time1 = time1;
    };

    Ray get_ray(double u, double v) const
    {
        Vec3 rd = lens_radius * random_in_unit_disk();
        Vec3 offset = this->u * rd.x() + this->v * rd.y();
        return Ray(origin + offset,
                   lower_left_corner + u * horizontal + v * vertical - origin - offset,
                   random_double(time0, time1));
    };

private:
    Vec3 origin;
    Vec3 lower_left_corner;
    Vec3 horizontal;
    Vec3 vertical;
    Vec3 u, v, w;
    double lens_radius;
    double viewport_width;
    double viewport_height;
    double time0, time1; // shutter open/close times
};

#endif // CAMERA_H