from cmath import inf
from math import sqrt
from tqdm import tqdm

import random

# local imports
from hittables import Hittable, HittableList, Material, Sphere
from camera import Camera
from hittable import Color, Ray, Vector, random_in_unit_sphere, outer
from hittables import Lambertian, Dielectric, Metal


def write_color(out_file, pixel_color: Color) -> None:
    """append an rbg color to the file"""

    max_rgb = 255.999

    # gamma correct for gamma=2
    red = max_rgb * sqrt(pixel_color.r())
    green = max_rgb * sqrt(pixel_color.g())
    blue = max_rgb * sqrt(pixel_color.b())

    rgb_string = f" {int(red)} {int(green)} {int(blue)}\n"
    out_file.write(rgb_string)


def ray_color(ray: Ray, world: HittableList, depth: int) -> Color:
    """computes rgb color of a ray"""

    # terminate recursion if ray bounce limit is reached, no light contribution
    if depth <= 0:
        return Color(0, 0, 0)

    # check and where world is hit
    hit_tolerance = 1e-3
    hit_record = world.hit(ray, hit_tolerance, inf)
    if hit_record is not None:
        is_scattered, scattered, attenuation = hit_record.material.scatter(
            ray, hit_record)
        if is_scattered:
            return outer(ray_color(scattered, world, depth-1), attenuation)
        else:
            return Color(0, 0, 0)

    unit_direction = ray.direction / ray.direction.length()
    t = 0.5*unit_direction.y() + 0.5

    return Color.from_vector(Vector(1, 1, 1) * (1 - t) + Vector(0.5, 0.7, 1.0) * t)


def random_scene() -> HittableList:
    ground_material = Lambertian(Color(0.5, 0.5, 0.5))
    world = HittableList(Sphere(Vector(0, -1000, 0), 1000, ground_material))

    for a in range(-11, 11):
        for b in range(-11, 11):
            choose_mat = random.random()
            center = Vector(a + 0.9*random.random(), 0.2,
                            b + 0.9*random.random())

            if (center - Vector(4, 0.2, 0)).length() > 0.9:

                if choose_mat < 0.8:
                    # diffuse
                    albedo = outer(Color.random(0, 1), Color.random(0, 1))
                    sphere_material = Lambertian(albedo)
                    world.add(Sphere(center, 0.2, sphere_material))
                elif choose_mat < 0.95:
                    # metal
                    albedo = Color.random(0.5, 1)
                    fuzz = random.random()/2
                    sphere_material = Metal(albedo, fuzz)
                    world.add(Sphere(center, 0.2, sphere_material))
                else:
                    # glass
                    sphere_material = Dielectric(1.5)
                    world.add(Sphere(center, 0.2, sphere_material))

    material1 = Dielectric(1.5)
    world.add(Sphere(Vector(0, 1, 0), 1.0, material1))

    material2 = Lambertian(Color(0.4, 0.2, 0.1))
    world.add(Sphere(Vector(-4, 1, 0), 1.0, material2))

    material3 = Metal(Color(0.7, 0.6, 0.5), 0.0)
    world.add(Sphere(Vector(4, 1, 0), 1.0, material3))

    return world


def main():
    """main function of the ray tracer"""

    def get_x_y(i: int, j: int) -> tuple:
        x = (i + random.random()) / (image_width - 1)
        y = (j + random.random()) / (image_height - 1)
        return x, y

    # Image
    aspect_ratio = 16/9
    image_width = 400
    image_height = int(image_width/aspect_ratio)
    number_samples = 20
    max_depth = 50

    world = random_scene()

    # set up camera
    look_from = Vector(13, 2, 3)
    look_at = Vector(0, 0, 0)
    view_up = Vector(0, 1, 0)
    aspect_ratio = 16/9
    vertical_field_of_view = 20
    distance_to_focus = 10
    aperture = 0.1

    camera = Camera(look_from, look_at, view_up, vertical_field_of_view,
                    aspect_ratio, aperture, distance_to_focus)

    # Render
    with open("image_test.ppm", "w+") as image_file:
        header = f"P3\n{image_width} {image_height}\n255\n"
        image_file.write(header)

        for j in tqdm(reversed(range(image_height)), total=image_height):
            for i in range(image_width):
                color = Color(0, 0, 0)
                for _ in range(number_samples):
                    u, v = get_x_y(i, j)
                    ray = camera.get_ray(u, v)
                    color = color + \
                        ray_color(ray, world, max_depth)/number_samples

                write_color(image_file, color)


if __name__ == '__main__':
    main()
