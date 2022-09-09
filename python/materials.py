import math
import random
from dataclasses import dataclass


# local imports
from cython_extension import Ray, Color, dot, unit_vector, random_unit_vector, reflect, refract, random_in_unit_sphere
from cython_extension import Material


@dataclass
class Lambertian(Material):
    """Diffusely reflecting material"""
    albedo: Color

    def scatter(self, ray_in, hit_record):
        # scatter in random direction
        scatter_direction = hit_record.normal + random_unit_vector()
        if scatter_direction.near_zero():
            scatter_direction = hit_record.normal

        scattered = Ray(hit_record.hit_point, scatter_direction, ray_in.time)

        return True, scattered, self.albedo


class Metal(Material):
    """Perfectly reflective material"""

    def __init__(self, albedo: Color, fuzz: float):
        self.albedo = albedo
        if fuzz < 1:
            self.fuzz = fuzz
        else:
            self.fuzz = 1

    def scatter(self, ray_in, hit_record):
        vector_in = unit_vector(ray_in.direction)
        # every ray is reflected
        reflected = reflect(vector_in, hit_record.normal)
        scattered = Ray(hit_record.hit_point, reflected +
                        random_in_unit_sphere()*self.fuzz, ray_in.time)
        is_scattered = dot(scattered.direction, hit_record.normal) > 0

        return is_scattered, scattered, self.albedo


@dataclass
class Dielectric(Material):
    """Transparent material"""
    index_of_refraction: float

    def scatter(self, ray_in, hit_record):
        attenuation = Color(1, 1, 1)
        if hit_record.hit_from_outside:
            refraction_ratio = 1/self.index_of_refraction
        else:
            refraction_ratio = self.index_of_refraction

        unit_direction = unit_vector(ray_in.direction)
        cos_theta = min(dot(unit_direction*-1, hit_record.normal), 1.0)
        sin_theta = math.sqrt(1.0-cos_theta**2)
        cannot_refract = refraction_ratio*sin_theta > 1
        reflectance = self.__reflectance(cos_theta, refraction_ratio)

        # decide if ray is reflected or refracted
        if cannot_refract or (reflectance > random.random()):
            direction = reflect(unit_direction, hit_record.normal)
        else:
            direction = refract(
                unit_direction, hit_record.normal, refraction_ratio)

        scattered_ray = Ray(hit_record.hit_point, direction, ray_in.time)
        is_scattered = True
        return is_scattered, scattered_ray, attenuation

    def __reflectance(self, cos_theta: float, refraction_ratio: float) -> float:
        """Computes Schlick's approximation of the fresnel factor"""
        r0 = (1-refraction_ratio) / (1+refraction_ratio)
        r0 = r0*r0
        return r0 + (1-r0)*(1-cos_theta)**5


@dataclass
class ReflectiveOpaque(Material):
    """Diffusely reflecting material with a very smooth surface"""
    albedo: Color
    index_of_refraction: float

    def scatter(self, ray_in, hit_record):
        attenuation = Color(1, 1, 1)
        if hit_record.hit_from_outside == 1:
            refraction_ratio = 1/self.index_of_refraction
        else:
            refraction_ratio = self.index_of_refraction

        unit_direction = unit_vector(ray_in.direction)
        cos_theta = min(dot(unit_direction*-1, hit_record.normal), 1.0)
        sin_theta = math.sqrt(1.0-cos_theta**2)
        cannot_refract = refraction_ratio*sin_theta > 1
        reflectance = self.__reflectance(cos_theta, refraction_ratio)
        if cannot_refract or (reflectance > random.random()):
            direction = reflect(unit_direction, hit_record.normal)
        else:
            direction = hit_record.normal + random_unit_vector()
            if direction.near_zero():
                direction = hit_record.normal

        scattered_ray = Ray(hit_record.hit_point, direction, ray_in.time)
        is_scattered = True
        return is_scattered, scattered_ray, attenuation

    def __reflectance(self, cos_theta: float, refraction_ratio: float) -> float:
        # Schlick's approximation
        r0 = (1-refraction_ratio) / (1+refraction_ratio)
        r0 = r0**2
        return r0 + (1-r0)*(1-cos_theta)**5


class DiffuseLight(Material):
    """Diffusely emmissive material"""

    def __init__(self, color: Color = Color(1, 1, 1)) -> None:
        self.color = color

    def scatter(self, ray_in, hit_record):
        is_scattered = False
        scattered_ray = None
        hit_record = None
        return is_scattered, scattered_ray, hit_record

    def emitted(self):
        return self.color
