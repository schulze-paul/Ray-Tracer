import math
import random

from sphere import Ray, Vector, unit_vector, random_in_unit_disk, cross


def degrees_to_radians(degrees: float):
    return degrees/180*math.pi


class Camera():
    aspect_ratio = 16/9
    viewport_height = 2.0
    viewport_width = viewport_height * aspect_ratio
    focal_length = 1.0

    origin = Vector(0, 0, 0)
    horizontal = Vector(viewport_width, 0, 0)
    vertical = Vector(0, viewport_height, 0)
    lower_left_corner = origin - horizontal/2 - \
        vertical/2 - Vector(0, 0, focal_length)

    def __init__(self, look_from: Vector, look_at: Vector, view_up: Vector, vertical_field_of_view: float, aspect_ratio: float, aperture: float, focus_distance, time0: float = 0, time1: float = 0):
        self.theta = degrees_to_radians(vertical_field_of_view)
        h = math.tan(self.theta/2)
        self.viewport_height = 2*h
        self.viewport_width = aspect_ratio*self.viewport_height

        # orthonormal basis
        self.w = unit_vector(look_from-look_at)
        self.u = unit_vector(cross(view_up, self.w))
        self.v = cross(self.w, self.u)

        self.origin = look_from
        self.horizontal = self.u * focus_distance*self.viewport_width
        self.vertical = self.v * focus_distance*self.viewport_height
        self.lower_left_corner = self.origin - self.horizontal / \
            2 - self.vertical/2 - self.w * focus_distance

        self.lens_radius = aperture/2
        self.time0 = time0
        self.time1 = time1

    def get_ray(self, x_pixel: float, y_pixel: float) -> Ray:
        random_origin = random_in_unit_disk() * self.lens_radius
        offset = self.u * random_origin.x() + self.v * random_origin.y()
        return Ray(self.origin + offset,
                   self.lower_left_corner +
                   self.horizontal * x_pixel +
                   self.vertical * y_pixel -
                   self.origin - offset,
                   self.random_time())

    def random_time(self):
        return self.time0 + (self.time1 + self.time0)*random.random()
