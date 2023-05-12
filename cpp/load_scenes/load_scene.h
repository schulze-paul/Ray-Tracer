/*
Load scene from yaml file

Example yaml file:
```yaml
# scene.yaml
# Scene description
#
# Camera
camera:
  lookfrom: [0, 0, 0]
  lookat: [0, 0, -1]
  vup: [0, 1, 0]
  vfov: 90
  aspect_ratio: 16/9
  aperture: 0.0
  focus_dist: 1.0
  time0: 0.0
  time1: 1.0
    samples_per_pixel: 100
    image_width: 400
# Objects
objects:
  - type: sphere
    center: [0, 0, -1]
    radius: 0.5
    material:
      type: lambertian
      albedo: [0.1, 0.2, 0.5]
  - type: sphere
    center: [0, -100.5, -1]
    radius: 100
    material:
      type: lambertian
      albedo: [0.8, 0.8, 0.0]
  - type: sphere
    center: [1, 0, -1]
    radius: 0.5
    material:
      type: metal
      albedo: [0.8, 0.6, 0.2]
      fuzz: 1.0
  - type: sphere
    center: [-1, 0, -1]
    radius: 0.5
    material:
      type: dielectric
      ir: 1.5
  - type: sphere
    center: [-1, 0, -1]
    radius: -0.45
    material:
      type: dielectric
      ir: 1.5
  - type: rectangle
    x0: [-1, 0, -1]
    x1: [1, 0, -1]
    y0: [-1, 0, -1]
    y1: [-1, 0, 1]
    k: -0.5
    material:
      type: diffuse_light
      color: [4, 4, 4]
  - type: rectangle
    x0: [-1, 0, -1]
    x1: [1, 0, -1]
    y0: [1, 0, -1]
    y1: [1, 0, 1]
    k: -0.5
    material:
      type: diffuse_light
      color: [4, 4, 4]
  - type: rectangle
    x0: [-1, 0, -1]
    x1: [1, 0, -1]
    y0: [-1, 0, 1]
    y1: [1, 0, 1]
    k: -0.5
    material:
      type: diffuse_light
      color: [4, 4, 4]
```

The file includes object geometries and materials as well as the camera setup and the image size.
*/


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

/**
 * @brief Load Vec3 from yaml node
 * @param vector yaml node
 * @return Vec3
*/
inline Vec3 load_vec3(YAML::Node &vector)    
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

/**
 * @brief Load Camera setup from yaml node.
 * @param camera Camera object
 * @param camera_data yaml node
*/
inline void load_camera(Camera &camera, YAML::Node &camera_data)
{   
    std::cerr << "loading camera" << std::endl;
    double vfov = camera_data["vfov"].as<double>();
    double aperture = camera_data["aperture"].as<double>();
    double aspect_ratio = camera_data["aspect_ratio"].as<double>();
    double focus_distance = camera_data["focal_length"].as<double>();
    double time0 = camera_data["time0"].as<double>();
    double time1 = camera_data["time1"].as<double>();
    int samples_per_pixel = camera_data["samples_per_pixel"].as<int>();
    int image_width = camera_data["image_width"].as<int>();
    
    auto look_from_data = camera_data["look_from"];
    Vec3 look_from = load_vec3(look_from_data); 
    auto look_at_data = camera_data["look_at"];
    Vec3 look_at = load_vec3(look_at_data);
    
    camera.set_up(vfov, aspect_ratio, aperture, focus_distance, samples_per_pixel, look_from, look_at, time0, time1);
    camera.set_image_data(image_width);
}

/**
 * @brief Load Sphere object from yaml node.
 * @param objects HittableList objects to add sphere to
 * @param sphere_data yaml node
 * @param material Material object to assign to sphere
*/
inline void load_sphere(HittableList &objects, YAML::Node &sphere_data, Material *material)
{
    std::cerr << "loading sphere" << std::endl;
    auto position_data = sphere_data["center"];
    Vec3 position = load_vec3(position_data);
    double radius = sphere_data["radius"].as<double>();
    auto sphere = std::make_shared<Sphere>(position, radius, material);
    objects.add(sphere);
}

/**
 * @brief Load XY Rectangle object from yaml node.
 * @param objects HittableList objects to add rectangle to
 * @param rectangle_data yaml node
 * @param material Material object to assign to rectangle
*/
inline void load_xy_rectangle(HittableList &objects, YAML::Node &rectangle_data, Material *material)
{
    std::cerr << "loading xy rectangle" << std::endl;
    double x0 = rectangle_data["x0"].as<double>();
    double x1 = rectangle_data["x1"].as<double>();
    double y0 = rectangle_data["y0"].as<double>();
    double y1 = rectangle_data["y1"].as<double>();
    double k = rectangle_data["k"].as<double>();
    auto rectangle = std::make_shared<XY_Rectangle>(x0, x1, y0, y1, k, material);
    objects.add(rectangle);
}

/**
 * @brief Load XZ Rectangle object from yaml node.
 * @param objects HittableList objects to add rectangle to
 * @param rectangle_data yaml node
 * @param material Material object to assign to rectangle
*/
inline void load_xz_rectangle(HittableList &objects, YAML::Node &rectangle_data, Material *material)
{
    std::cerr << "loading xz rectangle" << std::endl;
    double x0 = rectangle_data["x0"].as<double>();
    double x1 = rectangle_data["x1"].as<double>();
    double z0 = rectangle_data["z0"].as<double>();
    double z1 = rectangle_data["z1"].as<double>();
    double k = rectangle_data["k"].as<double>();
    auto rectangle = std::make_shared<XZ_Rectangle>(x0, x1, z0, z1, k, material);
    objects.add(rectangle);
}

/**
 * @brief Load YZ Rectangle object from yaml node.
 * @param objects HittableList objects to add rectangle to
 * @param rectangle_data yaml node
 * @param material Material object to assign to rectangle
*/
inline void load_yz_rectangle(HittableList &objects, YAML::Node &rectangle_data, Material *material)
{
    std::cerr << "loading yz rectangle" << std::endl;
    double y0 = rectangle_data["y0"].as<double>();
    double y1 = rectangle_data["y1"].as<double>();
    double z0 = rectangle_data["z0"].as<double>();
    double z1 = rectangle_data["z1"].as<double>();
    double k = rectangle_data["k"].as<double>();
    auto rectangle = std::make_shared<YZ_Rectangle>(y0, y1, z0, z1, k, material);
    objects.add(rectangle);
}

/**
 * @brief Load Box object from yaml node.
 * @param objects HittableList objects to add box to
 * @param box_data yaml node
 * @param material Material object to assign to box
*/
inline void load_box(HittableList &objects, YAML::Node &box_data, Material *material)
{
    std::cerr << "loading box" << std::endl;
    auto box_min_data = box_data["box_min"];
    auto box_max_data = box_data["box_max"];
    Vec3 box_min = load_vec3(box_min_data);
    Vec3 box_max = load_vec3(box_max_data);
    auto box = std::make_shared<Box>(box_min, box_max, material);
    objects.add(box);
}

inline void load_triangular_prism(HittableList &objects, YAML::Node &prism_data, Material *material)
{
    std::cerr << "loading triangular prism" << std::endl;
    auto v0_data = prism_data["v0"];
    auto v1_data = prism_data["v1"];
    auto v2_data = prism_data["v2"];
    auto v3_data = prism_data["v3"];
    auto v4_data = prism_data["v4"];
    auto v5_data = prism_data["v5"];
    Vec3 v0 = load_vec3(v0_data);
    Vec3 v1 = load_vec3(v1_data);
    Vec3 v2 = load_vec3(v2_data);
    Vec3 v3 = load_vec3(v3_data);
    Vec3 v4 = load_vec3(v4_data);
    Vec3 v5 = load_vec3(v5_data);
    auto prism = std::make_shared<TriangularPrism>(v0, v1, v2, v3, v4, v5, material);
    objects.add(prism);
}

/**
 * @brief load camera setup and objects from yaml file
 * @param filename yaml file to load
 * @param camera Camera object to load
 * @return HittableList of objects in scene
*/
HittableList load_scene(std::string filename, Camera &camera)
{
    std::ifstream fin(filename);
    // check if file exists
    if (!fin.good())
    {
        std::cerr << "File not accessible: " << filename << std::endl;
    }

    YAML::Node scene = YAML::Load(fin);

    auto camera_data = scene["scene"]["camera"];
    load_camera(camera, camera_data);

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
            Color color = load_vec3(color_data);
            material = new Lambertian(color);
        }
        else if (materialType.compare("metal") == 0)
        {
            auto metalData = objects_data[i]["material"];
            auto color_data = material_data["albedo"];
            Color color = load_vec3(color_data);
            double fuzz = material_data["fuzz"].as<double>();
            material = new Metal(color, fuzz);
        }
        else if (materialType.compare("dielectric") == 0)
        {
            auto dielectricData = objects_data[i]["material"];        
            double refraction_index = material_data["refraction_index"].as<double>();
            material = new Dielectric(refraction_index);
        }
        else if (materialType.compare("diffuse_light") == 0)
        {
            auto diffuseLightData = objects_data[i]["material"];
            auto color_data = material_data["color"];
            Color color = load_vec3(color_data);
            material = new DiffuseLight(color);
        }
        else {
            std::cerr << "Unknown material type: " << materialType << std::endl;
            exit(1);
        }
        materials.push_back(material);

        if (shapeType.compare("sphere") == 0)
        {
            auto sphere_data = objects_data[i];
            load_sphere(hittable_list, sphere_data, material);
        }
        else if (shapeType.compare("xy_rectangle") == 0)
        {
            auto rectangle_data = objects_data[i];
            load_xy_rectangle(hittable_list, rectangle_data, material);
        }
        else if (shapeType.compare("xz_rectangle") == 0)
        {
            auto rectangle_data = objects_data[i];
            load_xz_rectangle(hittable_list, rectangle_data, material);
        }
        else if (shapeType.compare("yz_rectangle") == 0)
        {
            auto rectangle_data = objects_data[i];
            load_yz_rectangle(hittable_list, rectangle_data, material);
        }
        else if (shapeType.compare("box") == 0)
        {
            auto box_data = objects_data[i];
            load_box(hittable_list, box_data, material);
        }
        else if (shapeType.compare("triangular_prism") == 0)
        {
            auto triangular_prism_data = objects_data[i];
            load_triangular_prism(hittable_list, triangular_prism_data, material);
        }
        else {
            std::cerr << "Unknown shape type: " << shapeType << std::endl;
            exit(1);
        }
    }

    std::cerr << material->to_string() << std::endl;
    

    auto objects = hittable_list.get_objects();

    // print data from hittable_list
    std::cerr << "objects size: " << objects.size() << std::endl;
    for (int i=0; i<objects.size(); i++) {

        objects[i]->set_material(materials.at(i));
        std::cerr << "object " << i << std::endl;
        std::cerr << objects[i]->to_string() << std::endl;
        std::cerr << "material: " << objects[i]->get_material()->to_string() << std::endl;
    }
    BVHNode node = BVHNode(objects, 0, objects.size(), 0, 1);
    return hittable_list;
}



