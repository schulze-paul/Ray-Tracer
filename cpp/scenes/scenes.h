#ifndef WORLD_H
#define WORLD_H

#include "hittable_list.h"
#include "sphere.h"
#include "materials.h"
#include "materials.cpp"
#include "camera.h"

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

HittableList get_random_scene(Camera &camera)
{
    camera = Camera(
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

HittableList get_random_scene_checker(Camera &camera)
{

    HittableList world;

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

#endif // WORLD_H