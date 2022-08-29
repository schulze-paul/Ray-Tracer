from cmath import inf
from math import sqrt
from tqdm import tqdm

from random import random

# local imports
from hittables import HittableList, Sphere
from camera import Camera
from ray import Color, Ray, Vector, random_in_unit_sphere, outer
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


def main():
    """main function of the ray tracer"""

    def get_x_y(i: int, j: int) -> tuple:
        x = (i + random()) / (image_width - 1)
        y = (j + random()) / (image_height - 1)
        return x, y

    # Image
    aspect_ratio = 16/9
    image_width = 400
    image_height = int(image_width/aspect_ratio)
    number_samples = 100
    max_depth = 50

    # World
    material_ground = Lambertian(Color(0.8, 0.8, 0.0))
    material_center = Lambertian(Color(0.1, 0.2, 0.5))
    material_left = Dielectric(1.5)
    material_right = Metal(Color(0.8, 0.6, 0.2), 0)

    world = HittableList(Sphere(Vector(0, -100.5, -1), 100, material_ground))
    world.add(Sphere(Vector(0, 0, -1), 0.5, material_center))
    world.add(Sphere(Vector(-1, 0, -1), 0.5, material_left))
    world.add(Sphere(Vector(-1, 0, -1), -0.4, material_left))
    world.add(Sphere(Vector(1, 0, -1), 0.5, material_right))

    camera = Camera()

    # Render
    with open("image.ppm", "w+") as image_file:
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
