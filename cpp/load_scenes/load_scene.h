#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <memory>
#include <vector>

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
#include "dielectric.h"

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


inline void load_sphere(HittableList &objects, YAML::Node &sphere_data, Material *material)
{
    std::cerr << "loading sphere" << std::endl;
    auto position_data = sphere_data["center"];
    Vec3 position = loadVec3(position_data);
    double radius = sphere_data["radius"].as<double>();
    auto material_data = sphere_data["material"];

    auto sphere = std::make_shared<Sphere>(position, radius, material);
    objects.add(sphere);
}

inline void load_xy_rectangle(HittableList &objects, YAML::Node &rectangle_data, Material *material)
{
    std::cerr << "loading xy rectangle" << std::endl;
    double x0 = rectangle_data["x0"].as<double>();
    double x1 = rectangle_data["x1"].as<double>();
    double y0 = rectangle_data["y0"].as<double>();
    double y1 = rectangle_data["y1"].as<double>();
    double k = rectangle_data["k"].as<double>();
    auto material_data = rectangle_data["material"];
    auto rectangle = std::make_shared<XY_Rectangle>(x0, x1, y0, y1, k, material);
    objects.add(rectangle);
}

inline void load_xz_rectangle(HittableList &objects, YAML::Node &rectangle_data, Material *material)
{
    std::cerr << "loading xz rectangle" << std::endl;
    double x0 = rectangle_data["x0"].as<double>();
    double x1 = rectangle_data["x1"].as<double>();
    double z0 = rectangle_data["z0"].as<double>();
    double z1 = rectangle_data["z1"].as<double>();
    double k = rectangle_data["k"].as<double>();
    auto material_data = rectangle_data["material"];
    auto rectangle = std::make_shared<XZ_Rectangle>(x0, x1, z0, z1, k, material);
    objects.add(rectangle);
}

inline void load_yz_rectangle(HittableList &objects, YAML::Node &rectangle_data, Material *material)
{
    std::cerr << "loading yz rectangle" << std::endl;
    double y0 = rectangle_data["y0"].as<double>();
    double y1 = rectangle_data["y1"].as<double>();
    double z0 = rectangle_data["z0"].as<double>();
    double z1 = rectangle_data["z1"].as<double>();
    double k = rectangle_data["k"].as<double>();
    auto material_data = rectangle_data["material"];
    auto rectangle = std::make_shared<YZ_Rectangle>(y0, y1, z0, z1, k, material);
    objects.add(rectangle);
}


HittableList load_scene(std::string filename, Camera &camera)
{
    std::ifstream fin(filename);

    YAML::Node scene = YAML::Load(fin);

    auto camera_data = scene["scene"]["camera"];
    loadCamera(camera, camera_data);

    HittableList hittable_list = HittableList();
    std::vector<Material *> materials;
    auto objects_data = scene["scene"]["objects"];
    int numberOfObjects = objects_data.size();
    std::string shapeType;
    std::string materialType;
    Material *material;

    for (int i=0; i<numberOfObjects; i++) 
    {
        shapeType = objects_data[i]["type"].as<std::string>();
        materialType = objects_data[i]["material"]["type"].as<std::string>();
        auto material_data = objects_data[i]["material"];

        if (materialType.compare("diffuse") == 0)
        {
            auto diffuseData = objects_data[i]["material"];
            auto color_data = material_data["color"];
            Color color = loadVec3(color_data);
            material = new Lambertian(color);
        }
        if (materialType.compare("metal") == 0)
        {
            auto metalData = objects_data[i]["material"];
            auto color_data = material_data["albedo"];
            Color color = loadVec3(color_data);
            double fuzz = material_data["fuzz"].as<double>();
            material = new Metal(color, fuzz);
        }
        if (materialType.compare("dielectric") == 0)
        {
            auto dielectricData = objects_data[i]["material"];        
            double refraction_index = material_data["refraction_index"].as<double>();
            material = new Dielectric(refraction_index);
        }
        if (materialType.compare("diffuse_light") == 0)
        {
            auto diffuseLightData = objects_data[i]["material"];
            auto color_data = material_data["color"];
            Color color = loadVec3(color_data);
            material = new DiffuseLight(color);
        }
        materials.push_back(material);

        if (shapeType.compare("sphere") == 0)
        {
            auto sphere_data = objects_data[i];
            load_sphere(hittable_list, sphere_data, material);
        }
        if (shapeType.compare("xy_rectangle") == 0)
        {
            auto rectangle_data = objects_data[i];
            load_xy_rectangle(hittable_list, rectangle_data, material);
        }
        if (shapeType.compare("xz_rectangle") == 0)
        {
            auto rectangle_data = objects_data[i];
            load_xz_rectangle(hittable_list, rectangle_data, material);
        }
        if (shapeType.compare("yz_rectangle") == 0)
        {
            auto rectangle_data = objects_data[i];
            load_yz_rectangle(hittable_list, rectangle_data, material);
        }
    }

    std::cerr << material->toString() << std::endl;
    

    auto objects = hittable_list.getObjects();

    // print data from hittable_list
    std::cerr << "objects size: " << objects.size() << std::endl;
    for (int i=0; i<objects.size(); i++) {

        objects[i]->setMaterial(materials.at(i));
        std::cerr << "object " << i << std::endl;
        std::cerr << objects[i]->toString() << std::endl;
        std::cerr << "material: " << objects[i]->getMaterial()->toString() << std::endl;
    }
    BVHNode node = BVHNode(objects, 0, objects.size(), 0, 1);
    return hittable_list;
}



