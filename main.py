import sys
from cmath import inf
from math import sqrt
from time import time
from tqdm import tqdm
import numpy as np
import random
import matplotlib.pyplot as plt
import pickle
import multiprocessing as mp

# local imports
from python.hittables import Hittable, HittableList, Sphere, MovableSphere, Box
from python.camera import Camera
from hittable import Color, Ray, Vector, random_in_unit_sphere, outer, RectangleXY, RectangleYZ, RectangleZX
from python.hittables import Lambertian, Dielectric, Metal, ReflectiveOpaque, DiffuseLight, Material, BoundingVolumeHierarchyNode


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

    if hit_record is None:
        unit_direction = ray.direction / ray.direction.length()
        t = 0.5*unit_direction.data[1] + 0.5

        background = Color.from_vector(
            Vector(0.2, 0.2, 0.2) * (1 - t) + Vector(0.3, 0.3, 0.3)*0.5 * t)

        return background

    emitted = hit_record.material.emitted()

    is_scattered, scattered, attenuation = hit_record.material.scatter(
        ray, hit_record)
    if not is_scattered:
        return emitted
    else:
        return emitted + outer(ray_color(scattered, world, depth-1), attenuation)


def random_scene() -> HittableList:
    walls_color = Color(1, 1, 1)

    ground_material = Lambertian(walls_color)
    world = HittableList(Sphere(Vector(0, -1000, 0), 1000, ground_material))

    """
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
    """

    # big spheres
    large_radius = 1.2

    material1 = Lambertian(Color(1, 1, 1))
    world.add(Sphere(Vector(-3, large_radius, 0), large_radius, material1))

    material2 = ReflectiveOpaque(Color(1, 1, 1), 1.5)
    world.add(Sphere(Vector(0, large_radius, 0), large_radius, material2))

    material3 = Metal(Color(1, 1, 1), 0.0)
    world.add(Sphere(Vector(3, large_radius, 0), large_radius, material3))

    # small  colored spheres
    small_radius = 0.5

    material_red = Lambertian(Color(1, 0, 0))
    world.add(Sphere(Vector(-3, small_radius, 2),
                     small_radius, material_red))

    material_blue = Lambertian(Color(0, 0, 1))
    world.add(Sphere(Vector(3, small_radius, 2),
                     small_radius, material_blue))

    material_green = Lambertian(Color(0, 1, 0))
    world.add(Sphere(Vector(0, small_radius, 2),
                     small_radius, material_green))

    # small glass balls

    material_glass = Dielectric(1.5)
    world.add(Sphere(Vector(-1.5, small_radius, 2),
              small_radius, material_glass))

    material_diamond = Dielectric(2.4)
    world.add(Sphere(Vector(1.5, small_radius, 2),
              small_radius, material_diamond))

    # add light

    material_light = DiffuseLight(Vector(3, 3, 3))
    world.add(Sphere(Vector(-8, 8, 0), 2, material_light))

    # add walls
    material_wall = Lambertian(walls_color)
    world.add(RectangleXY(-10, 10, 0, 6, -10, material_wall))

    world.add(RectangleYZ(0, 6, -10, 10, 10, material_wall))

    # add box
    world.add(Box(Vector(2, 0, 5), Vector(3, 1, 6), material_wall))

    return BoundingVolumeHierarchyNode(world.hittable_objects, 0, 1)
    # return world


def world_on_cube():

    def get_cube_filled_spheres(num_spheres: int, point0: Vector, point1: Vector, sphere_radius: float, material: Material):
        spheres = []

        for _ in range(num_spheres):
            x = random.uniform(point0.data[0], point1.data[0])
            y = random.uniform(point0.data[1], point1.data[1])
            z = random.uniform(point0.data[2], point1.data[2])

            sphere = Sphere(Vector(x, y, z), sphere_radius, material)

            spheres.append(sphere)

        return spheres

    def get_cube(centerpoint: Vector, width: float, material: Material):
        return Box(Vector(centerpoint.data[0] - width/2, centerpoint.data[1] - width/2, centerpoint.data[2] - width/2),
                   Vector(centerpoint.data[0] + width/2, centerpoint.data[1] +
                          width/2, centerpoint.data[2] + width/2),
                   material)

    # colors
    walls_color = Color(1, 1, 1)
    light_color = Color(1, 1, 0.8)
    # materials

    material_wall = Lambertian(walls_color)
    material_light = DiffuseLight(light_color*8)
    material_lightbox = DiffuseLight(Color(1, 1, 0.8)*2)
    material_mirror = Metal(Color(1, 1, 1), 0)

    material_brushed_metal = Metal(Color(1, 1, 1), 1)

    material_glass = Dielectric(1.5)
    material_diamond = Dielectric(2.4)

    material_smooth = ReflectiveOpaque(Color(1, 1, 1), 1.5)

    material_red = Lambertian(Color(1, 0, 0))
    material_blue = Lambertian(Color(0, 0, 1))
    material_green = Lambertian(Color(0, 1, 0))

    # measurements
    base_cube_width = 20

    small_cube_width = 2
    small_radius = small_cube_width/2
    large_radius = small_cube_width

    # objects

    # base cube

    world = HittableList(
        Box(Vector(-base_cube_width/2, -base_cube_width, -base_cube_width/2), Vector(base_cube_width/2-small_cube_width*2, 0, base_cube_width/2), material_wall))
    world.add(
        Box(Vector(-base_cube_width/2, -base_cube_width, -base_cube_width/2), Vector(base_cube_width/2, 0, base_cube_width/2-small_cube_width*2), material_wall))

    world.add(
        Box(Vector(-base_cube_width/2, -base_cube_width, -base_cube_width/2), Vector(base_cube_width/2, 0-small_cube_width*2, base_cube_width/2), material_wall))

    # boxes
    world.add(get_cube(Vector(0, 0, 0),
              small_cube_width*2, material_wall))
    world.add(get_cube(Vector(-small_cube_width*2, 0, small_cube_width*2),
              small_cube_width*2, material_wall))
    world.add(get_cube(Vector(small_cube_width*2, 0, -small_cube_width*2),
              small_cube_width*2, material_wall))

    # side boxes
    world.add(Box(Vector(base_cube_width/2, 0, 0),
              Vector(base_cube_width/2+small_cube_width, small_cube_width, -small_cube_width), material_wall))
    world.add(Box(Vector(base_cube_width/2, -small_cube_width, 0),
              Vector(base_cube_width/2+small_cube_width, 0, small_cube_width), material_wall))
    world.add(Box(Vector(base_cube_width/2, -2*small_cube_width, small_cube_width),
              Vector(base_cube_width/2+small_cube_width, -1*small_cube_width, 2*small_cube_width), material_wall))

    # colored spheres
    world.add(Sphere(Vector(base_cube_width/2 + small_radius, -small_radius+2*small_cube_width, small_radius-small_cube_width),
                     small_radius, material_red))
    world.add(Sphere(Vector(base_cube_width/2 + small_radius, -small_radius+small_cube_width, small_radius),
                     small_radius, material_blue))
    world.add(Sphere(Vector(base_cube_width/2 + small_radius, -small_radius, small_radius+small_cube_width),
                     small_radius, material_green))

    # light sphere
    world.add(Sphere(Vector(base_cube_width/2 + -2*small_radius, -small_radius-small_cube_width, base_cube_width/2-2*small_radius),
                     small_radius, material_light))

    # large spheres
    world.add(Sphere(Vector(-small_cube_width*2, large_radius+small_cube_width, small_cube_width*2),
              large_radius, material_wall))
    world.add(Sphere(Vector(0, large_radius+small_cube_width, 0),
              large_radius, material_smooth))
    world.add(Sphere(Vector(small_cube_width*2, large_radius+small_cube_width, -small_cube_width*2),
              large_radius, material_mirror))

    # glass spheres
    world.add(Sphere(Vector(base_cube_width/2-3*small_cube_width, 2*small_radius, base_cube_width/2-small_cube_width),
              small_radius, material_glass))
    world.add(Sphere(Vector(base_cube_width/2-small_cube_width, 2*small_radius, base_cube_width/2-3*small_cube_width),
              small_radius, material_diamond))

    # light
    # world.add(Box(Vector(-base_cube_width/2, 0, base_cube_width/2),
    #          Vector(-base_cube_width/2+small_cube_width, base_cube_width/2, base_cube_width/2 - small_cube_width), material_glass))
    world.add(get_cube(Vector(small_cube_width*2, -small_cube_width+0.01, 0),
              small_cube_width*2, material_lightbox))
    world.add(get_cube(Vector(0, -small_cube_width+0.01, small_cube_width*2),
              small_cube_width*2, material_lightbox))

    world.add(Box(Vector(5*small_cube_width, 12*small_cube_width, 5*small_cube_width),
              Vector(-5*small_cube_width, 12*small_cube_width, -5*small_cube_width), material_light))

    # spheres in a cube
    spheres_radius = 0.5
    spheres = get_cube_filled_spheres(200,
                                      Vector(-base_cube_width/2+small_cube_width*2,
                                             2*small_cube_width, -base_cube_width/2+small_cube_width*2),
                                      Vector(-base_cube_width/2, 4*small_cube_width, -base_cube_width/2), spheres_radius, material_wall)
    for sphere in spheres:
        world.add(sphere)

    return BoundingVolumeHierarchyNode(world.hittable_objects, 0, 1)
    # return world


def one_ray_per_pixel(camera, world, max_depth, image_width, image_height):
    def get_x_y(i: int, j: int) -> tuple:
        x = (i + random.random()) / (image_width - 1)
        y = (j + random.random()) / (image_height - 1)
        return x, y

    image = np.zeros((image_width, image_height, 3))
    for j in reversed(range(image_height)):
        for i in range(image_width):
            u, v = get_x_y(i, j)
            ray = camera.get_ray(u, v)
            color = ray_color(ray, world, max_depth)
            image[i, j] = np.array(
                [color.data[0], color.data[1], color.data[2]])

    return image


def main(debug=False):
    """main function of the ray tracer"""

    # Image
    aspect_ratio = 2/3
    image_width = 400
    image_height = int(image_width/aspect_ratio)
    number_samples = 5
    max_depth = 16

    world = world_on_cube()

    # set up camera
    look_from = Vector(50, 40, 30)
    look_at = Vector(0, 0, 0)
    view_up = Vector(0, 1, 0)
    vertical_field_of_view = 20
    distance_to_focus = look_from.length()
    aperture = 0.1

    camera = Camera(look_from, look_at, view_up, vertical_field_of_view,
                    aspect_ratio, aperture, distance_to_focus)

    image = np.zeros((image_width, image_height, 3))

    # Render
    cpu_count = mp.cpu_count()
    inputs = (camera, world, max_depth, image_width, image_height)

    if not debug:
        with mp.Pool() as pool:
            results = []
            for _ in range(cpu_count):
                results.append(pool.apply_async(one_ray_per_pixel, inputs))

            for result in tqdm(results):
                image += result.get()

        image /= cpu_count
    else:
        time_a = time()
        image = one_ray_per_pixel(
            camera, world, max_depth, image_width, image_height)
        print(time() - time_a)

    plt.imshow(np.rot90(image))
    plt.xticks([])
    plt.yticks([])

    plt.savefig("image_low_iterations.png")


if __name__ == '__main__':
    main(True)
