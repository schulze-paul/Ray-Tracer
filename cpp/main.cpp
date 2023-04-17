#include <iostream>

#include "ray_tracer.h"
#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "hit_record.h"
#include "hittable_list.h"
#include "camera.h"
#include "image_data.h"
#include "scenes.h"

Color background_color(const Ray &r)
{
    double y_alignment = unit_vector(r.direction).y();
    double normalized_y_alignment = 0.5f * (y_alignment + 1.0f);
    Color top_color = Color(0.5f, 0.7f, 1.0f);
    Color bottom_color = Color(1.0f, 1.0f, 1.0f);
    return (1.0f - normalized_y_alignment) * bottom_color + normalized_y_alignment * top_color;
}

Color get_ray_color(const Ray &r, HittableList &world, int depth)
{
    HitRecord rec;
    // max depth reached
    if (depth <= 0)
    {
        return Color(0, 0, 0);
    }

    // hit something
    if (world.hit(r, 0.0f, 100.0f, rec))
    {
        Ray scattered;
        Color attenuation;
        if (rec.getMaterial()->scatter(r, rec, attenuation, scattered))
        {
            return attenuation * get_ray_color(scattered, world, depth - 1);
        }
        return 0.5f * Color(rec.getNormal().x() + 1.0f, rec.getNormal().y() + 1.0f, rec.getNormal().z() + 1.0f);
    }

    // background
    return background_color(r);
}

int main()
{
    // world
    Camera camera;
    HittableList world = get_random_scene_checker(camera);

    ImageData image = ImageData(
        400,        // width
        16.0 / 9.0, // aspect ratio
        128,        // samples per pixel
        500);       // max depth

    // loop over image
    for (int j = image.get_height() - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image.get_width(); ++i)
        {
            for (int s = 0; s < image.get_samples_per_pixel(); ++s)
            {
                // ray
                double u = image.get_u(i);
                double v = image.get_v(j);
                Ray ray = camera.get_ray(u, v);

                Color color = get_ray_color(ray, world, image.get_max_depth());
                image.add_color(i, j, color);
            }
        }
    }

    std::cerr << std::endl
              << "saving image..."
              << std::endl;
    image.write_ppm(std::cout);
    return 0;
}
