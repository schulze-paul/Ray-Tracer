import numpy as np
from ray import Ray


class Camera():
    aspect_ratio = 16/9
    viewport_height = 2.0
    viewport_width = viewport_height * aspect_ratio
    focal_length = 1.0

    __origin = np.array([0, 0, 0])
    __horizontal = np.array([viewport_width, 0, 0])
    __vertical = np.array([0, viewport_height, 0])
    __lower_left_corner = __origin - __horizontal/2 - \
        __vertical/2 - np.array([0, 0, focal_length])

    def get_ray(self, x_pixel: float, y_pixel: float) -> Ray:
        return Ray(self.__origin,
                   self.__lower_left_corner +
                   x_pixel * self.__horizontal +
                   y_pixel * self.__vertical)
