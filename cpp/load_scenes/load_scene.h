#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <memory>

#include "yaml-cpp/yaml.h" // reading yaml file

#include "bvh.h"
#include "camera.h"
#include "materials.h"
#include "lambertian.h"
#include "metal.h"
#include "diffuse_light.h"
#include "rectangle.h"
#include "texture.h"
#include "sphere.h"

inline Vec3 loadVec3(YAML::Node &vector)    
{
    double x = vector[0].as<double>();
    double y = vector[1].as<double>();
    double z = vector[2].as<double>();

    Vec3 vec3 = Vec3();
    vec3[0] = x;
    vec3[1] = y;
    vec3[2] = z;
    return vec3;
}

inline void loadCamera(Camera &camera, YAML::Node &camera_data)
{   
    std::cerr << "loading camera" << std::endl;
    double vfov = camera_data["vfov"].as<double>();
    double aperture = camera_data["aperture"].as<double>();
    double aspect_ratio = camera_data["aspect_ratio"].as<double>();
    double focus_distance = camera_data["focus_dist"].as<double>();
    double time0 = camera_data["time0"].as<double>();
    double time1 = camera_data["time1"].as<double>();

    auto look_from_data = camera_data["look_from"];
    Vec3 look_from = loadVec3(look_from_data); 
    auto look_at_data = camera_data["look_at"];
    Vec3 look_at = loadVec3(look_at_data);
    
    camera.setUp(vfov, aspect_ratio, aperture, focus_distance, look_from, look_at, time0, time1);
}

inline void load_material(std::shared_ptr<Material> material, YAML::Node &material_data) 
{
    std::cerr << "loading material" << std::endl;
    std::string type = material_data["type"].as<std::string>();
    std::string diffuse = "diffuse";
    if (type.compare("diffuse")) {
        auto color_data = material_data["color"];
        Color color = loadVec3(color_data);
        material = std::make_shared<Lambertian>(color);
    }
}

inline void load_sphere(std::shared_ptr<HittableList> objects, YAML::Node &sphere_data)
{
    std::cerr << "loading sphere" << std::endl;
    auto position_data = sphere_data["center"];
    Vec3 position = loadVec3(position_data);
    double radius = sphere_data["radius"].as<double>();
    auto material_data = sphere_data["material"];
    std::shared_ptr<Material> material;
    load_material(material, material_data);
    auto sphere = std::make_shared<Sphere>(position, radius, material);
    objects->add(sphere);
}

inline void load_objects(std::shared_ptr<HittableList> objects, YAML::Node &objects_data)
{
    int numberOfObjects = objects_data.size();
    std::string type;

    for (int i=0; i<numberOfObjects; i++) 
    {
        type = objects_data[i]["type"].as<std::string>();
        if (type.compare("sphere") == 0)
        {
            auto sphere_data = objects_data[i];
            load_sphere(objects, sphere_data);
        } 
    }
}

BVHNode load_scene(std::string filename, Camera &camera)
{
    std::ifstream fin(filename);

    YAML::Node scene = YAML::Load(fin);

    auto camera_data = scene["scene"]["camera"];
    loadCamera(camera, camera_data);

    auto hittable_list = std::make_shared<HittableList>();
    auto object_data = scene["scene"]["objects"];
    load_objects(hittable_list, object_data);

    auto objects = hittable_list->getObjects();

    // print data from hittable_list
    std::cerr << "objects size: " << objects.size() << std::endl;
    for (int i=0; i<objects.size(); i++) {
        std::cerr << "object " << i << std::endl;
        std::cerr << objects[i]->toString() << std::endl;
    }

    return BVHNode(objects, 0, objects.size(), 0, 1);
}



