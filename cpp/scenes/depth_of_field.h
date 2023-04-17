#include <memory>

#include "hittable_list.h"
#include "color.h"
#include "camera.h"
#include "sphere.h"
#include "vec3.h"

#include "material.h"
#include "lambertian.h"
#include "camera.h"

HittableList get_depth_of_field_example(Camera &camera)
{
    Vec3 look_from(3, 3, 2);
    Vec3 look_at(0, 0, -1);
    double dist_to_focus = (look_from - look_at).length();
    double aperture = 2.0;

    camera = Camera(
        20,            // vertical field of view
        16.0 / 9.0,    // aspect ratio
        aperture,      // aperture
        dist_to_focus, // focus distance
        look_from,     // origin
        look_at        // look at
    );

    Material *material_ground = new Lambertian(Color(0.8, 0.8, 0.0));
    Material *material_blue = new Lambertian(Color(0.1, 0.2, 0.5));
    Material *material_glass = new Dielectric(1.5);
    Material *material_metal = new Metal(Color(0.8, 0.6, 0.2), 0.0);

    auto sphere_ground = std::make_shared<Sphere>(Vec3(0.0, -100.5, -1.0), 100.0, material_ground);
    auto sphere_glass = std::make_shared<Sphere>(Vec3(-1.0, 0.0, -1.0), 0.5, material_glass);
    auto sphere_glass_air = std::make_shared<Sphere>(Vec3(-1.0, 0.0, -1.0), -0.45, material_glass);
    auto sphere_blue = std::make_shared<Sphere>(Vec3(0.0, 0.0, -1.0), 0.5, material_blue);
    auto sphere_metal = std::make_shared<Sphere>(Vec3(1.0, 0.0, -1.0), 0.5, material_metal);

    HittableList world = HittableList();
    world.add(sphere_ground);
    world.add(sphere_glass);
    world.add(sphere_glass_air);
    world.add(sphere_blue);
    world.add(sphere_metal);
    return world;
}
