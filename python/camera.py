import math
import random

from hittable import Ray, Vector, unit_vector, random_in_unit_disk, cross


def degrees_to_radians(degrees: float):
    return degrees/180*math.pi


class Camera():
    """
    Class to store data about the camera that captures a scene.

    Use get_ray(x, y) to get a random ray for the pixel of the 
    image at position x, y in the image. 
    """

    def __init__(self,
                 look_from: Vector,
                 look_at: Vector,
                 view_up: Vector,
                 vertical_field_of_view: float,
                 aspect_ratio: float,
                 aperture: float,
                 focus_distance: float,
                 time0: float = 0,
                 time1: float = 0):

        self.theta = degrees_to_radians(vertical_field_of_view)
        h = math.tan(self.theta/2)
        self.aspect_ratio = aspect_ratio
        self.viewport_height = 2*h
        self.viewport_width = self.aspect_ratio*self.viewport_height

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
        """Get a random ray for the pixel x, y in the image."""
        random_origin = random_in_unit_disk() * self.lens_radius
        offset = self.u * random_origin.x() + self.v * random_origin.y()
        return Ray(self.origin + offset,
                   self.lower_left_corner +
                   self.horizontal * x_pixel +
                   self.vertical * y_pixel -
                   self.origin - offset,
                   self.random_time())

    def random_time(self):
        """Get a random time in the exposure time interval."""
        return self.time0 + (self.time1 + self.time0)*random.random()
