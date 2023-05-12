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
#include "spectra.h"

void find_light_sources(HittableList &world, std::shared_ptr<HittableList>& lights)
{
    for (int i = 0; i < world.size(); i++)
    {
        // add all non-lambertian materials to lights
        // metals, dielectrics and lights
        if (!world.get(i)->get_material()->is_lambertian())
        {
            lights->add(world.get(i));
            std::cout << "found light/metal/glass in material " << i << std::endl;
        }

    }

    for (int j = 0; j < world.size(); j++)
    {
        // set lights for lambertian materials
        Lambertian *lambertian = static_cast<Lambertian*>(world.get(j)->get_material());
        if (lambertian->is_lambertian())
        {
            std::cout << "setting lights for material " << j << std::endl;

            lambertian->set_lights(lights);
        }
    }
    
}



int main(int argc, char *argv[]) 
{
    std::string in_file_name = argv[1];
    std::string out_file_name = argv[2];

    std::cerr << "Loading scene: " << in_file_name << std::endl;
    std::cerr << "Output file: " << out_file_name << std::endl;
    
    // world
    Camera camera;
    HittableList world = load_scene(in_file_name, camera);
    auto night_background = SolidBackground(Color(0.0, 0.0, 0.0));
    auto day_background = GradientBackground(Color(0.05, 0.07, 0.01), Color(0.1, 0.1, 0.1));
    
    auto lights = std::make_shared<HittableList>(); 
    find_light_sources(world, lights);
    
    Color color;

    Color distance_color;
    ImageData distance_image(camera.image.get_width(), camera.image.get_height());
    Color normal_color;
    ImageData normal_image(camera.image.get_width(), camera.image.get_height());

    IntensitySpectrum spectrum;
    SpectralImageData spectral_image(camera.image.get_width(), camera.image.get_height());

    int max_depth = 16;

    ProgressBar bar(camera.samples_per_pixel);
    for (int s = 0; s < camera.samples_per_pixel; ++s)
    {
        for (int j = camera.image.get_height() - 1; j >= 0; --j)
        {
            for (int i = 0; i < camera.image.get_width(); ++i)
            {
                // new ray
                double u = camera.image.get_u(i);
                double v = camera.image.get_v(j);
                Ray ray = camera.get_ray(u, v);

                // compute color / spectrum for pixel
                color = ray_tracing_shader(ray, world, night_background, max_depth);
                //spectrum = spectral_ray_tracing_shader(ray, world, night_background, max_depth);
                normal_color = normal_shader(ray, world, night_background, max_depth);
                distance_color = distance_shader(ray, world, night_background, max_depth);

                // add to image
                camera.image.add_color(i, j, color);
                //spectral_image.add_spectrum(i, j, spectrum);
                if (s == 0)
                {
                    normal_image.add_color(i, j, normal_color);
                    distance_image.add_color(i, j, distance_color);
                }
            }
        }
        bar.increment();     
    }

    // post processing
    distance_image.normalize();
    //camera.image.gamma_correct(2.2);

    // write to file
    camera.image.write_to_ppm(out_file_name);
    spectral_image.write_to_ppm("spectral_" + out_file_name);
    normal_image.write_to_ppm("normal_" + out_file_name);
    distance_image.write_to_ppm("distance_" + out_file_name);
    return 0;
}



int main_number_wavelength_samples()
{
    int max_num_samples = 1000;
    int image_height = 100;
    ImageData image(max_num_samples, image_height);
    IntensitySpectrum spectrum = get_black_body_spectrum(WHITE_POINT_TEMPERATURE);
    IntensitySpectrum temp_spectrum;
    int wavelength_index;


    for (size_t num_samples = 1; num_samples <= max_num_samples; num_samples++)
    {
        for (size_t i = 0; i < image_height; i++)
        {
            temp_spectrum = IntensitySpectrum();
            for (size_t j = 0; j < num_samples; j++)
            {
                wavelength_index = spectrum.sample_wavelength();
                temp_spectrum[wavelength_index] += spectrum[wavelength_index];
            }
            image.add_color(num_samples-1, i, temp_spectrum.to_rgb());
        }
    }
    image.write_to_ppm("white_point_sampling.ppm");

    return 0;
}


int main_check_white_point()
{
    IntensitySpectrum whitest_spectrum;
    double whitest_temperture = 10000;
    Color whitest_color = Color(0.0, 0.0, 0.0);
    Color white = Color(1.0, 1.0, 1.0);
    
    Color color;
    IntensitySpectrum spectrum;

    double start_temperture = 1000;
    double end_temperture = 10000;
    double temperture = start_temperture;
    double temperture_step = 1000;
    double temperture_step_size = (end_temperture - start_temperture) / temperture_step;

    ImageData image((int)temperture_step, 300);

    for (size_t i = 0; i < temperture_step; i++)
    {
        temperture += temperture_step_size;
        spectrum = get_black_body_spectrum(temperture);
        color = spectrum.to_rgb();
        for (size_t j = 0; j < 100; j++)
        {
            image.add_color(i, j, color);
        }

        if ((color-white).length_squared() < (whitest_color-white).length_squared())
        {
            whitest_spectrum = spectrum;
            whitest_color = color;
            whitest_temperture = temperture;
        }

        spectrum = spectrum * 0.5;
        color = spectrum.to_rgb();
        for (size_t j = 100; j < 200; j++)
        {
            image.add_color(i, j, color);
        }
    }
    for (size_t i = 0; i < temperture_step; i++)
    {
        for (size_t j = 200; j < 300; j++)
        {
            image.add_color(i, j, whitest_color);
        }
    }

    
    std::cout << "whitest temperture: " << whitest_temperture << std::endl;
    std::cout << "whitest color: " << whitest_color << std::endl;
    std::cout << "Power of whitest color: " << whitest_spectrum.get_power() << std::endl;
    image.write_to_ppm("black_body.ppm");
    
    return 0;
}


int main_temp(int argc, char *argv[]) 
{
    std::string in_file_name = argv[1];
    std::string out_file_name = argv[2];

    std::cerr << "Loading scene: " << in_file_name << std::endl;
    std::cerr << "Output file: " << out_file_name << std::endl;
    
    // world
    Camera camera;
    HittableList world = load_scene(in_file_name, camera);
    auto night_background = SolidBackground(Color(0.0, 0.0, 0.0));
    auto day_background = GradientBackground(Color(0.05, 0.07, 0.01), Color(0.1, 0.1, 0.1));
    
    auto lights = std::make_shared<HittableList>(); 
    find_light_sources(world, lights);
    
    ProgressBar bar(camera.image.get_height());
    Color color;
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

                color = ray_tracing_shader(ray, world, night_background, 16);

                // discard NANs
                if (color.x() != color.x()) color = Color(0,0,0);
                if (color.y() != color.y()) color = Color(0,0,0);
                if (color.z() != color.z()) color = Color(0,0,0);
                camera.image.add_color(i, j, color);
            }
        }
    }

    camera.image.write_to_ppm(out_file_name);
    return 0;
}
