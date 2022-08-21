from ray import Ray
from vector_cython import Vector


class Camera():
    aspect_ratio = 16/9
    viewport_height = 2.0
    viewport_width = viewport_height * aspect_ratio
    focal_length = 1.0

    __origin = Vector(0, 0, 0)
    __horizontal = Vector(viewport_width, 0, 0)
    __vertical = Vector(0, viewport_height, 0)
    __lower_left_corner = __origin - __horizontal/2 - \
        __vertical/2 - Vector(0, 0, focal_length)

    def __init__(self) -> None:
        pass

    def get_ray(self, x_pixel: float, y_pixel: float) -> Ray:
        return Ray(self.__origin,
                   self.__lower_left_corner +
                   self.__horizontal * x_pixel +
                   self.__vertical * y_pixel)
