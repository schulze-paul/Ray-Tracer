from ray import Ray
from vector_cython import Vector


class Camera():

    __origin = Vector(0, 0, 0)
    __horizontal = Vector(viewport_width, 0, 0)
    __vertical = Vector(0, viewport_height, 0)
    __lower_left_corner = __origin - __horizontal/2 - \
        __vertical/2 - Vector(0, 0, focal_length)

    def __init__(look_from: Vector, look_at: Vector, view_up: Vector, vertical_field_of_view: float, aspect_ratio: float, aperture: float, focus_distance):
        self.theta = degrees_to_radians(vertical_field_of_view)
        h = math.tan(theta/2)
        self.viewport_height = 2*h
        self.viewport_width = aspect_ratio*viewport_height
        
        # orthonormal basis
        self.w = unit_vector(look_from-look_at)
        self.u = unit_vector(cross(view_up, self.w))
        self.v = cross(self.w, self.u)
        
        self.__origin = lookfrom
        self.__horizontal = focus_distance*viewport_width * u
        self.__vertical = focus_distance*viewport_height * v
        self.__lower_left_corner = origin - horizontal/2 - vertical/2 - focus_distance*w
        
        self.lens_radius = aperture/2

    def get_ray(self, x_pixel: float, y_pixel: float) -> Ray:
        random_origin = self.lens_radius*random_in_unit_disk()
        offset = self.u * random_origin.x() + self.v * random_origin.y()
        return Ray(self.__origin + offset,
                   self.__lower_left_corner +
                   self.__horizontal * x_pixel +
                   self.__vertical * y_pixel - 
                   self.__origin - offset)
