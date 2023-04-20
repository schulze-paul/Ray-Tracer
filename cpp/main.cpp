#include <iostream>
#include <fstream>
#include <cmath>


#include "ray_tracer.h"
#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "hit_record.h"
#include "hittable_list.h"
#include "camera.h"
#include "image_data.h"
#include "bvh.cpp"
#include "rectangle.h"
#include "progress_bar.h"
#include "load_scene.h"


Color night_sky(const Ray &r)
{
    return Color(0, 0, 0);
}

Color day_sky(const Ray &r)
{
    Vec3 unit_direction = unit_vector(r.direction);
    float t = 0.5f * (unit_direction.y() + 1.0f);
    return (1.0f - t) * Color(1.0f, 1.0f, 1.0f) + t * Color(0.5f, 0.7f, 1.0f);
}

Color ray_tracing_shader(const Ray &r, HittableList &world, int depth)
{
    HitRecord rec;
    // max depth reached
    if (depth <= 0)
    {
        return Color(0, 0, 0);
    }

    // background
    if (!world.hit(r, 0.001f, 100.0f, rec))
    {
        return night_sky(r);
    }

    // hit something
    Ray scattered;
    Color attenuation;
    Color emitted = rec.getMaterial()->emitted(rec.getU(), rec.getV(), rec.getHitPoint());
    if (!rec.getMaterial()->scatter(r, rec, attenuation, scattered))
    {
        return emitted;
    }

    return emitted + attenuation * ray_tracing_shader(scattered, world, depth - 1);
}

Color scattering_shader(const Ray &r, HittableList &world, int depth)
{
    HitRecord rec = HitRecord();
    if (world.hit(r, 0.001f, 100.0f, rec))
    {
        Ray scattered;
        Color attenuation;

        if (rec.getMaterial()->scatter(r, rec, attenuation, scattered)) 
        {
            return scattered.direction*0.5 + Vec3(0.5,0.5, 0.5);
        }
    }
    
    return night_sky(r);
}


Color normal_shader(const Ray &r, HittableList &world, int depth)
{
    HitRecord rec;
    if (!world.hit(r, 0.001f, 100.0f, rec))
    {
        return night_sky(r);
    }
    
    Vec3 normal = rec.isFrontFace(r) ? rec.getNormal() : -rec.getNormal();
    return normal*0.5 + Vec3(0.5, 0.5, 0.5);
}


int main()
{
    // world
    Camera camera;
    HittableList world = load_scene("scenes/sphere_lighting.yaml", camera);

    ImageData image = ImageData(
        400,        // width
        camera.aspect_ratio, // aspect ratio
        1024,        // samples per pixel
        50);       // max depth

    int total = image.get_height();
    ProgressBar bar(total);
    int progress = 0;
    // loop over image
    for (int j = image.get_height() - 1; j >= 0; --j)
    {
        bar.increment();
        for (int i = 0; i < image.get_width(); ++i)
        {
            // progress bar
            for (int s = 0; s < image.get_samples_per_pixel(); ++s)
            {
                // ray
                double u = image.get_u(i);
                double v = image.get_v(j);
                Ray ray = camera.get_ray(u, v);

                Color color = ray_tracing_shader(ray, world, image.get_max_depth());
                image.add_color(i, j, color);
            }
        }
    }

    image.write_to_file("sphere_lighting_back_wall.ppm");
    return 0;
}
