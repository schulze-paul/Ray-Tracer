from ray import Ray
from ray import Vector


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

    def __init__(self) -> None:
        pass

    def get_ray(self, x_pixel: float, y_pixel: float) -> Ray:
        return Ray(self.origin,
                   self.lower_left_corner +
                   self.horizontal * x_pixel +
                   self.vertical * y_pixel)
