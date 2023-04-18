
#include <memory>

#include "hittable.h"
#include "bvh.h"
#include "aabb.h"
#include "hittable_list.h"
#include "rectangle.h"
#include "sphere.h"
#include "materials.h"
#include "lambertian.h"
#include "diffuse_light.h"
#include "rectangle.h"
#include "texture.h"
#include "solid_color.h"
#include "camera.h"
#include "translation.h"
#include "rotation.h"


BVHNode cornell_box(Camera &camera)
{
    HittableList world;

    auto red   = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = std::make_shared<DiffuseLight>(Color(15, 15, 15));
    auto metal = std::make_shared<Metal>(Color(1.0, 1.0, 1.0), 0.0);

    world.add(std::make_shared<YZ_Rectangle>(0, 555, 0, 555, 555, green));
    world.add(std::make_shared<YZ_Rectangle>(0, 555, 0, 555, 0, red));
    world.add(std::make_shared<XZ_Rectangle>(213, 343, 227, 332, 554, light));
    world.add(std::make_shared<XZ_Rectangle>(0, 555, 0, 555, 0, metal));
    world.add(std::make_shared<XZ_Rectangle>(0, 555, 0, 555, 555, white));
    world.add(std::make_shared<XY_Rectangle>(0, 555, 0, 555, 555, white));

    
    std::shared_ptr<Hittable> box1 = std::make_shared<Box>(Vec3(0, 0, 0), Vec3(165, 330, 165), white);
    box1 = std::make_shared<RotateY>(box1, 15);
    box1 = std::make_shared<Translate>(box1, Vec3(265,0,295));
    world.add(box1);

    std::shared_ptr<Hittable> box2 = std::make_shared<Box>(Vec3(0, 0, 0), Vec3(165, 165, 165), white);
    box2 = std::make_shared<RotateY>(box2, -18);
    box2 = std::make_shared<Translate>(box2, Vec3(130,0,65));
    world.add(box2);

    // add a sphere in front of the boxes
    std::shared_ptr<Hittable> sphere = std::make_shared<Sphere>(Vec3(278+130,25,0+65), 50, light);
    world.add(sphere);

    auto objects = world.getObjects();
    int n = objects.size();
    BVHNode bvh(objects, 0, n, 0, 1);

    camera = Camera(
        40,                     // vfov
        1.0,             // aspect ratio
        0.1,                    // aperture
        1000,                   // focus_dist
        Vec3(278, 278, -800),   // lookfrom
        Vec3(278, 278, 0),      // lookat
        0,                      // time0
        1);                     // time1

    return bvh;
}	
