/*
Camera.h
========

The Camera class is used to represent a camera in the scene.
It can generate rays from the camera to the scene.
It also saves the image data.
*/

#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "ray.h"
#include "color.h"
#include "image_data.h"

/**
 * @brief      Class for camera.
 */
class Camera
{
public:
    Camera(){};
    Camera(double vfov, double aspect_ratio, double aperture, double focus_dist, int samples_per_pixel, Vec3 look_from, Vec3 look_at, double time0 = 0, double time1 = 0)
    {
        set_up(vfov, aspect_ratio, aperture, focus_dist, samples_per_pixel, look_from, look_at, time0, time1);
    };

    /**
     * @brief      Sets up the camera.
     * @param[in]  vfov              The vertical field of view
     * @param[in]  aspect_ratio      The aspect ratio
     * @param[in]  aperture          The aperture
     * @param[in]  focus_dist        The focus distance
     * @param[in]  samples_per_pixel The number of samples per pixel
     * @param[in]  look_from         The location of the camera
     * @param[in]  look_at           The location the camera is looking at
     * @param[in]  time0             The time 0
     * @param[in]  time1             The time 1
    */
    void set_up(double vfov, double aspect_ratio, double aperture, double focus_dist, int samples_per_pixel, Vec3 look_from, Vec3 look_at, double time0 = 0, double time1 = 0)
    {
        double theta = degrees_to_radians(vfov);
        double h = tan(theta / 2);
        this->viewport_height = 2.0 * h;
        this->viewport_width = aspect_ratio * viewport_height;
        this->aspect_ratio = aspect_ratio;

        this->w = unit_vector(look_from - look_at);
        this->u = unit_vector(cross(Vec3(0, 1, 0), w));
        this->v = cross(w, u);

        this->look_from = look_from;
        this->look_at = look_at;
        this->horizontal = focus_dist * viewport_width * u;
        this->vertical = focus_dist * viewport_height * v;
        this->lower_left_corner = look_from - horizontal / 2 - vertical / 2 - focus_dist * w;

        this->samples_per_pixel = samples_per_pixel;
        this->lens_radius = aperture / 2;
        this->time0 = time0;
        this->time1 = time1;
    };

    /**
     * @brief      Sets the image data.
     * @param[in]  image_width  The image width
     */
    void set_image_data(int image_width)
    {
        this->image = ImageData(image_width, aspect_ratio);
    }

    /**
     * @brief      Generates a ray from the camera to the scene.
     * @param[in]  u     The horizontal coordinate
     * @param[in]  v     The vertical coordinate
     * @return     A ray from the camera to the scene.
    */
    Ray get_ray(double u, double v) const
    {
        Vec3 rd = lens_radius * random_in_unit_disk();
        Vec3 offset = this->u * rd.x() + this->v * rd.y();
        return Ray(look_from + offset,
                   lower_left_corner + u * horizontal + v * vertical - look_from - offset,
                   random_double(time0, time1));
    };

public:
    Vec3 look_from;
    Vec3 look_at;
    Vec3 lower_left_corner;
    Vec3 horizontal;
    Vec3 vertical;
    Vec3 u, v, w;
    double lens_radius;
    double viewport_width;
    double viewport_height;
    double time0, time1; // shutter open/close times
    double aspect_ratio;
    int samples_per_pixel;
    ImageData image;
};

#endif // CAMERA_H