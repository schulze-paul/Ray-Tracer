#include <iostream>
#include <fstream>
#include <cmath>
#include <string_view>

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

using namespace std::literals;

void find_light_sources(HittableList &world, std::shared_ptr<HittableList>& lights)
{
    for (int i = 0; i < world.size(); i++)
    {
        if (world.get(i)->get_material()->is_emissive())
            lights->add(world.get(i));
    }
    for (int j = 0; j < world.size(); j++)
    {
        // set lights for lambertian materials
        Lambertian *lambertian = static_cast<Lambertian*>(world.get(j)->get_material());
        if (lambertian->is_lambertian())
        {
            std::cerr << "setting lights for material " << j << std::endl;

            lambertian->set_lights(lights);
        }
    }
}

class InputParser{
    public:
        InputParser (int &argc, char **argv){
            for (int i=1; i < argc; ++i)
                this->tokens.push_back(std::string(argv[i]));
        }
        /// @author iain
        const std::string& getCmdOption(const std::string &option) const{
            std::vector<std::string>::const_iterator itr;
            itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
            if (itr != this->tokens.end() && ++itr != this->tokens.end()){
                return *itr;
            }
            static const std::string empty_string("");
            return empty_string;
        }
        /// @author iain
        bool cmdOptionExists(const std::string &option) const{
            return std::find(this->tokens.begin(), this->tokens.end(), option)
                   != this->tokens.end();
        }
    private:
        std::vector <std::string> tokens;
};



int main(int argc, char** argv) 
{
    
    // Parse input
    // -----------------------------------------------------------------
    InputParser input(argc, argv);
    if(input.cmdOptionExists("-h"))
    {
        std::cout << "List of commands:..." << std::endl;
        std::cout << " -h: Help" << std::endl;
        std::cout << " -i: Input scene file (.yaml)" << std::endl;
        std::cout << " -o: Output file name (.ppm)" << std::endl;
        std::cout << " -v: Verbose" << std::endl;
        std::cout << " -n: Number of samples per pixel" << std::endl;
        std::cout << " -s: Shader (normal, depth, scatter)" << std::endl;
        return 0;
    }
    
    const std::string in_file_name = input.getCmdOption("-i");
    const std::string out_file_name = input.getCmdOption("-o");
    if (in_file_name.empty() || out_file_name.empty())
    {
        // Do interesting things ...
        std::cout << "Specify input and output file names with -i and -o." << std::endl;
        return 1;
    }
    
    // samples per pixel
    int samples_per_pixel = 1048;
    const std::string num_samples_string = input.getCmdOption("-n");
    if (!num_samples_string.empty())
    {
        samples_per_pixel = std::stoi(num_samples_string);
    }
    else
    {
        std::cerr << "Number of samples not specified: Default=" << samples_per_pixel << std::endl;
    }

    std::cerr << "Loading scene: " << in_file_name << std::endl;
    std::cerr << "Output file: " << out_file_name << std::endl;

    // shader
    const std::string shader = input.getCmdOption("-s");

    
    // Load Scene
    // -----------------------------------------------------------------
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
            for (int s = 0; s < samples_per_pixel; ++s)
            {
                // ray
                double u = camera.image.get_u(i);
                double v = camera.image.get_v(j);
                Ray ray = camera.get_ray(u, v);

                if (shader.empty()) 
                {
                    color = ray_tracing_shader(ray, world, night_background, 16);
                }
                else if (shader == "normal")
                {
                    color = normal_shader(ray, world, night_background, 16);
                }
                else if (shader == "depth")
                {
                    color = depth_shader(ray, world, night_background, 16);
                }
                else if (shader == "scatter")
                {
                    color = scattering_shader(ray, world, night_background, 16);
                }
                else if (shader == "cosine")
                {
                    color = cosine_pdf_ray_tracing_shader(ray, world, night_background, 16);
                }
                else if (shader == "lights")
                {
                    color = light_pdf_ray_tracing_shader(ray, world, night_background, 16, lights);
                }
                
                // discard NANs
                if (color.x() != color.x()) color = Color(0,0,0);
                if (color.y() != color.y()) color = Color(0,0,0);
                if (color.z() != color.z()) color = Color(0,0,0);
                camera.image.add_color(i, j, color);
            }
        }
    }
    if (shader == "depth") {
        camera.image *= 1/camera.image.get_max();
    }

    camera.image.write_to_ppm(out_file_name);
    return 0;
}
