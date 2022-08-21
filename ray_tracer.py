from cmath import inf
from tqdm import tqdm

from random import random

# local imports
from hittables import HittableList, Sphere
from ray import Ray
from camera import Camera
from vector_cython import Color, random_in_unit_sphere, Vector


def write_color(out_file, pixel_color: Color, number_samples: int) -> None:
    """append an rbg color to the file"""
    pixel_color /= number_samples

    max_rgb = 255.999

    red = max_rgb * pixel_color.r()
    green = max_rgb * pixel_color.g()
    blue = max_rgb * pixel_color.b()

    rgb_string = f" {int(red)} {int(green)} {int(blue)}\n"
    out_file.write(rgb_string)


def ray_color(ray: Ray, world: HittableList, depth: int) -> Color:
    """computes rgb color of a ray"""

    # terminate recursion if ray bounce limit is reached, no light contribution
    if depth <= 0:
        return Color(0, 0, 0)

    # check and where world is hit
    hit_record = world.hit(ray, 0, inf)
    if hit_record is not None:
        target = hit_record.hit_point + hit_record.normal + random_in_unit_sphere()
        return ray_color(
            Ray(hit_record.hit_point, target - hit_record.hit_point),
            world,
            depth - 1)/2

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
    world = HittableList(Sphere(Vector(0, 0, -1), 0.5))
    world.add(Sphere(Vector(0, -100.5, -1), 100))

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

                write_color(image_file, color, number_samples)


if __name__ == '__main__':
    main()
