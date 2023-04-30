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

void find_light_sources(HittableList &world, std::shared_ptr<HittableList>& lights)
{
    for (int i = 0; i < world.size(); i++)
    {
        if (world.get(i)->getMaterial()->isEmissive())
            lights->add(world.get(i));
    }
}


int main(int argc, char *argv[]) 
{
    std::string inFileName = argv[1];
    std::string outFileName = argv[2];

    std::cerr << "Loading scene: " << inFileName << std::endl;
    std::cerr << "Output file: " << outFileName << std::endl;
    
    // world
    Camera camera;
    HittableList world = load_scene(inFileName, camera);
    auto night_background = SolidBackground(Color(0.0, 0.0, 0.0));
    auto day_background = GradientBackground(Color(0.05, 0.07, 0.01), Color(0.1, 0.1, 0.1));
    
    auto lights = std::make_shared<HittableList>(); 
    find_light_sources(world, lights);
    
    ProgressBar bar(camera.image.get_height());
    for (int j = camera.image.get_height() - 1; j >= 0; --j)
    {
        bar.increment();
        for (int i = 0; i < camera.image.get_width(); ++i)
        {
            // progress bar
            for (int s = 0; s < camera.samples_per_pixel; ++s)
            {
                // ray
                double u = camera.image.get_u(i);
                double v = camera.image.get_v(j);
                Ray ray = camera.get_ray(u, v);

                Color color = ray_tracing_shader(ray, world, lights, night_background, 16);

                // discard NANs
                if (color.x() != color.x()) color = Color(0,0,0);
                if (color.y() != color.y()) color = Color(0,0,0);
                if (color.z() != color.z()) color = Color(0,0,0);

                camera.image.add_color(i, j, color);
            }
        }
    }

    camera.image.write_to_file(outFileName);
    return 0;
}
