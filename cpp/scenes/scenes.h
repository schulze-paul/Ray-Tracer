#ifndef WORLD_H
#define WORLD_H

#include "hittable_list.h"
#include "color.h"
#include "vec3.h"

#include "materials.h"
#include "lambertian.h"
#include "metal.h"
#include "dielectric.h"

#include "sphere.h"
#include "moving_sphere.h"
#include "camera.h"

#include "texture.h"
#include "solid_color.h"
#include "checker_texture.h"
#include "noise_texture.h"

#include "bvh.h"

HittableList get_world()
{
    HittableList world = HittableList();

    Color ground_color = Color(0.8, 0.8, 0.0);
    Color green_color = Color(0.0, 0.8, 0.0);
    Material *ground_material = new Lambertian(ground_color);
    Material *green_material = new Lambertian(green_color);
    auto ground = std::make_shared<Sphere>(Vec3(0, -100, 0), 100, ground_material);
    auto green_sphere = std::make_shared<Sphere>(Vec3(0, 1, 0), 1, green_material);
    world.add(ground);
    world.add(green_sphere);

    return world;
}

HittableList get_random_scene(Camera &camera)
{
    camera.setUp(
        20,             // vertical field of view
        16.0 / 9.0,     // aspect ratio
        0.1,            // aperture
        10.0,           // focus distance
        Vec3(13, 2, 3), // origin
        Vec3(0, 0, 0)   // look at
    );

    HittableList world;

    Material *ground_material = new Lambertian(Color(0.5, 0.5, 0.5));
    auto ground = std::make_shared<Sphere>(Vec3(0, -1000, 0), 1000, ground_material);
    world.add(ground);

    // random spheres
    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            auto choose_material = random_double();
            Vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - Vec3(4, 0.2, 0)).length() > 0.9)
            {
                Material *sphere_material;

                if (choose_material < 0.8)
                {
                    // diffuse
                    auto albedo = Color::random(0, 1) * Color::random(0, 1);
                    sphere_material = new Lambertian(albedo);
                }
                else if (choose_material < 0.95)
                {
                    // metal
                    auto albedo = Color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = new Metal(albedo, fuzz);
                }
                else
                {
                    // glass
                    sphere_material = new Dielectric(1.5);
                }
                auto sphere = std::make_shared<Sphere>(center, 0.2, sphere_material);
                world.add(sphere);
            }
        }
    }

    Material *material1 = new Dielectric(1.5);
    auto sphere1 = std::make_shared<Sphere>(Vec3(0, 1, 0), 1.0, material1);
    world.add(sphere1);

    Material *material2 = new Lambertian(Color(0.4, 0.2, 0.1));
    auto sphere2 = std::make_shared<Sphere>(Vec3(-4, 1, 0), 1.0, material2);
    world.add(sphere2);

    Material *material3 = new Metal(Color(0.7, 0.6, 0.5), 0.0);
    auto sphere3 = std::make_shared<Sphere>(Vec3(4, 1, 0), 1.0, material3);
    world.add(sphere3);

    return world;
}

HittableList get_random_scene_moving(Camera &camera)
{

    camera.setUp(
        20,             // vertical field of view
        16.0 / 9.0,     // aspect ratio
        0.1,            // aperture
        10.0,           // focus distance
        Vec3(13, 2, 3), // origin
        Vec3(0, 0, 0),  // look at
        0,              // time 0
        1               // time 1
    );
    std::cerr << "Moving scenario" << std::endl;

    HittableList world;

    Material *ground_material = new Lambertian(Color(0.5, 0.5, 0.5));
    auto ground = std::make_shared<Sphere>(Vec3(0, -1000, 0), 1000, ground_material);
    world.add(ground);

    // random spheres
    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            auto choose_material = random_double();
            Vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - Vec3(4, 0.2, 0)).length() > 0.9)
            {
                Material *sphere_material;

                if (choose_material < 0.8)
                {
                    // diffuse
                    auto albedo = Color::random(0, 1) * Color::random(0, 1);
                    sphere_material = new Lambertian(albedo);
                    center = center + Vec3(0, random_double(0, 0.5), 0);
                    auto sphere = std::make_shared<MovingSphere>(center, center + Vec3(0, 0.5, 0), 0.0, 1.0, 0.2, sphere_material);
                }
                else if (choose_material < 0.95)
                {
                    // metal
                    auto albedo = Color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = new Metal(albedo, fuzz);
                    auto sphere = std::make_shared<Sphere>(center, 0.2, sphere_material);
                }
                else
                {
                    // glass
                    sphere_material = new Dielectric(1.5);
                    auto sphere = std::make_shared<Sphere>(center, 0.2, sphere_material);
                }
                // world.add(sphere);
            }
        }
    }

    Material *material1 = new Dielectric(1.5);
    auto sphere1 = std::make_shared<Sphere>(Vec3(0, 1, 0), 1.0, material1);
    world.add(sphere1);

    Material *material2 = new Lambertian(Color(0.4, 0.2, 0.1));
    auto sphere2 = std::make_shared<Sphere>(Vec3(-4, 1, 0), 1.0, material2);
    world.add(sphere2);

    Material *material3 = new Metal(Color(0.7, 0.6, 0.5), 0.0);
    auto sphere3 = std::make_shared<Sphere>(Vec3(4, 1, 0), 1.0, material3);
    world.add(sphere3);

    return world;
}

BVHNode get_random_scene_checker(Camera &camera)
{

    camera.setUp(
        20,             // vertical field of view
        16.0 / 9.0,     // aspect ratio
        0.1,            // aperture
        10.0,           // focus distance
        Vec3(13, 2, 3), // origin
        Vec3(0, 0, 0),  // look at
        0,              // time 0
        1               // time 1
    );

    HittableList world;

    std::cerr << "Checker scenario" << std::endl;

    Texture *texture_a = new SolidColor(Color(0.2, 0.3, 0.1));
    Texture *texture_b = new SolidColor(Color(0.9, 0.9, 0.9));
    Texture *checker = new CheckerTexture(texture_a, texture_b);
    Material *ground_material = new Lambertian(checker);
    auto ground = std::make_shared<Sphere>(Vec3(0, -1000, 0), 1000, ground_material);
    world.add(ground);

    // random spheres
    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            auto choose_material = random_double();
            Vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - Vec3(4, 0.2, 0)).length() > 0.9)
            {
                Material *sphere_material;
                if (choose_material < 0.8)
                {
                    // diffuse
                    auto albedo = Color::random(0, 1) * Color::random(0, 1);
                    sphere_material = new Lambertian(albedo);
                    center = center + Vec3(0, random_double(0, 0.5), 0);
                    auto sphere = std::make_shared<MovingSphere>(center, center + Vec3(0, 0.5, 0), 0.0, 1.0, 0.2, sphere_material);
                    world.add(sphere);
                }
                else if (choose_material < 0.95)
                {
                    // metal
                    auto albedo = Color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = new Metal(albedo, fuzz);
                    auto sphere = std::make_shared<Sphere>(center, 0.2, sphere_material);
                    world.add(sphere);
                }
                else
                {
                    // glass
                    sphere_material = new Dielectric(1.5);
                    auto sphere = std::make_shared<Sphere>(center, 0.2, sphere_material);
                    world.add(sphere);
                }
            }
        }
    }

    Material *material1 = new Dielectric(1.5);
    auto sphere1 = std::make_shared<Sphere>(Vec3(0, 1, 0), 1.0, material1);
    world.add(sphere1);

    Material *material2 = new Lambertian(Color(0.4, 0.2, 0.1));
    auto sphere2 = std::make_shared<Sphere>(Vec3(-4, 1, 0), 1.0, material2);
    world.add(sphere2);

    Material *material3 = new Metal(Color(0.7, 0.6, 0.5), 0.0);
    auto sphere3 = std::make_shared<Sphere>(Vec3(4, 1, 0), 1.0, material3);
    world.add(sphere3);

    auto objects = world.getObjects();

    return BVHNode(objects, 0, objects.size(), 0, 1);
}

#endif // WORLD_H