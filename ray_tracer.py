from dataclasses import dataclass
from tqdm import tqdm
import numpy as np


def write_color(out_file, pixel_color: np.ndarray) -> None:
    red = int(255.999*pixel_color[0])
    green = int(255.999*pixel_color[1])
    blue = int(255.999*pixel_color[2])

    rgb_string = f" {red} {green} {blue}\n"
    out_file.write(rgb_string)


@dataclass
class Ray():
    """ray class """
    origin: np.ndarray
    direction: np.ndarray

    def at(self, t: float) -> np.ndarray:
        """get position of ray at t"""
        return self.origin + t*self.direction


def ray_color(ray: Ray) -> np.ndarray:
    """computes rgb color of a ray"""

    # check and where if sphere is hit
    sphere_origin = np.array([0, 0, -1])
    sphere_radius = 0.5
    t = hit_sphere(sphere_origin, sphere_radius, ray)

    # if sphere was hit:
    if t > 0:
        normal = ray.at(t) - np.array([0, 0, -1])
        unit_normal = normal / np.linalg.norm(normal)
        return 0.5*np.array([unit_normal[0]+1, unit_normal[1]+1, unit_normal[2]+1])

    # draw background gradient
    unit_direction = ray.direction / np.linalg.norm(ray.direction)
    t = 0.5*unit_direction[1] + 0.5
    return (1 - t) * np.array([1.0, 1.0, 1.0]) + t*np.array([0.5, 0.7, 1.0])


def hit_sphere(center: np.ndarray, radius: float, ray: Ray) -> float:
    """
    computes if the given ray hits the sphere:

    the result depends on the root of this equation
    t^2*b*b + 2*t*b*(A-C) + (A-C)*(A-C)-r^2=0
    """
    origin_to_center = ray.origin - center
    a = np.dot(ray.direction, ray.direction)
    b = 2.0 * np.dot(origin_to_center, ray.direction)
    c = np.dot(origin_to_center, origin_to_center) - radius**2
    discriminant = b*b - 4*a*c

    if discriminant < 0:
        # no solution
        return -1
    else:
        # get t of closest hit point
        return (-b - np.sqrt(discriminant)) / (2*a)


def main():
    """main function of the ray tracer"""
    # Image
    aspect_ratio = 16/9
    image_width = 1000
    image_height = int(image_width/aspect_ratio)

    # Camera
    viewport_height = 2.0
    viewport_width = aspect_ratio * viewport_height
    focal_length = 1.0

    origin = np.array([0, 0, 0])
    horizontal = np.array([viewport_width, 0, 0])
    vertical = np.array([0, viewport_height, 0])
    lower_left_corner = origin - horizontal/2 - \
        vertical/2 - np.array([0, 0, focal_length])

    # Render
    with open("image.ppm", "w+") as image_file:
        header = f"P3\n{image_width} {image_height}\n255\n"
        image_file.write(header)

        for j in tqdm(reversed(range(image_height)), total=image_height):
            for i in range(image_width):
                u = float(i / (image_width-1))
                v = float(j / (image_height-1))

                ray = Ray(origin, direction=lower_left_corner +
                          u*horizontal + v*vertical - origin)

                color = ray_color(ray)

                write_color(image_file, color)


if __name__ == '__main__':
    main()
