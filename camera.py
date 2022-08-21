from ray import Ray
from vector_utils import Vector


class Camera():
    aspect_ratio = 16/9
    viewport_height = 2.0
    viewport_width = viewport_height * aspect_ratio
    focal_length = 1.0

    __origin = Vector()
    __horizontal = Vector(x=viewport_width)
    __vertical = Vector(y=viewport_height)
    __lower_left_corner = __origin - __horizontal/2 - \
        __vertical/2 - Vector(z=focal_length)

    def __init__(self) -> None:
        pass

    def get_ray(self, x_pixel: float, y_pixel: float) -> Ray:
        return Ray(self.__origin,
                   self.__lower_left_corner +
                   x_pixel * self.__horizontal +
                   y_pixel * self.__vertical)
