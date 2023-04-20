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
#include "background.h"
#include "shaders.h"


int main()
{
    // world
    Camera camera;
    HittableList world = load_scene("scenes/sphere_lighting.yaml", camera);
    auto night_background = SolidBackground(Color(0.0, 0.0, 0.0));
    auto day_background = GradientBackground(Color(0.5, 0.7, 1.0), Color(1.0, 1.0, 1.0));


    ImageData image = ImageData(
        400,        // width
        camera.aspect_ratio, // aspect ratio
        128,        // samples per pixel
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

                Color color = ray_tracing_shader(ray, world, day_background, image.get_max_depth());
                image.add_color(i, j, color);
            }
        }
    }

    image.write_to_file("background_test.ppm");
    return 0;
}
