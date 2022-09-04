from dataclasses import dataclass
import math
import random
import numpy as np

# local imports
from hittable import Ray, Vector, Color, dot, unit_vector, random_unit_vector, reflect, refract, random_in_unit_sphere
# , Lambertian, Dielectric, Metal
from hittable import HitRecord, Hittable, Material, Sphere, MovableSphere  # , HittableList

class HittableList(Hittable):
    """Implementing hit function for a collection of objects."""

    def __init__(self, hittable: Hittable) -> None:
        self.hittable_objects = [hittable]

    def add(self, hittable: Hittable) -> None:
        self.hittable_objects.append(hittable)

    def hit(self, ray, t_min, t_max) -> HitRecord:
        """Compute closest hit of ray to any object."""
        closest_hit_record = None
        
        for hittable in self.hittable_objects:
            hit_record = hittable.hit(ray, t_min, t_max)
            if hit_record is not None:
                
                if closest_hit_record is None:
                    closest_hit_record = hit_record
                elif closest_hit_record.t > hit_record.t:
                    closest_hit_record = hit_record
        
        return closest_hit_record


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
        reflected = reflect(vector_in, hit_record.normal) # every ray is reflected
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


@dataclass
class RectangleXY(Hittable):
    """Rectangle in the XY plane."""
    x0: float
    x1: float
    y0: float
    y1: float
    k: float
    material: Material

    def hit(self, ray, t_min, t_max) -> HitRecord:
        t_hit = (self.k - ray.origin.z()) / ray.direction.z()
        if t_hit < t_min or t_hit > t_max:
            return None

        x = ray.origin.x() + ray.direction.x() * t_hit
        y = ray.origin.y() + ray.direction.y() * t_hit

        if (x < self.x0 or x > self.x1 or y < self.y0 or y > self.y1):
            return None

        # collect hit data
        hit_point = ray(t_hit)
        surface_normal = Vector(0, 0, 1)

        hit_record = HitRecord(hit_point, t_hit, self.material)
        hit_record.set_face_normal(ray, surface_normal)

        return hit_record


@dataclass
class RectangleYZ(Hittable):
    """Rectangle in the YZ plane."""
    y0: float
    y1: float
    z0: float
    z1: float
    k: float
    material: Material

    def hit(self, ray, t_min, t_max) -> HitRecord:
        t_hit = (self.k - ray.origin.x()) / ray.direction.x()
        if t_hit < t_min or t_hit > t_max:
            return None

        y = ray.origin.y() + ray.direction.y() * t_hit
        z = ray.origin.z() + ray.direction.z() * t_hit

        if (y < self.y0 or y > self.y1 or z < self.z0 or z > self.z1):
            return None

        # collect hit data
        hit_point = ray(t_hit)
        surface_normal = Vector(1, 0, 0)

        hit_record = HitRecord(hit_point, t_hit, self.material)
        hit_record.set_face_normal(ray, surface_normal)

        return hit_record


@dataclass
class RectangleZX(Hittable):
    """Rectangle in the ZX plane."""
    z0: float
    z1: float
    x0: float
    x1: float
    k: float
    material: Material

    def hit(self, ray, t_min, t_max) -> HitRecord:
        t_hit = (self.k - ray.origin.y()) / ray.direction.y()
        if t_hit < t_min or t_hit > t_max:
            return None

        z = ray.origin.z() + ray.direction.z() * t_hit
        x = ray.origin.x() + ray.direction.x() * t_hit

        if (z < self.z0 or z > self.z1 or x < self.x0 or x > self.x1):
            return None

        hit_point = ray(t_hit)
        surface_normal = Vector(0, 1, 0)

        hit_record = HitRecord(hit_point, t_hit, self.material)
        hit_record.set_face_normal(ray, surface_normal)

        return hit_record


class Box(Hittable):
    """Rectangular cuboid."""
    def __init__(self, point0: Vector, point1: Vector, material: Material) -> None:
        min_x, max_x = sorted((point0.x(), point1.x()))
        min_y, max_y = sorted((point0.y(), point1.y()))
        min_z, max_z = sorted((point0.z(), point1.z()))

        self.minimum_point = Vector(min_x, min_y, min_z)
        self.maximum_point = Vector(max_x, max_y, max_z)

        # shape is composed of six rectangles
        self.sides = HittableList(RectangleXY(
            min_x, max_x, min_y, max_y, max_z, material))
        self.sides.add(RectangleXY(
            min_x, max_x, min_y, max_y, min_z, material))

        self.sides.add(RectangleYZ(
            min_y, max_y, min_z, max_z, max_x, material))
        self.sides.add(RectangleYZ(
            min_y, max_y, min_z, max_z, min_x, material))

        self.sides.add(RectangleZX(
            min_z, max_z, min_x, max_x, max_y, material))
        self.sides.add(RectangleZX(
            min_z, max_z, min_x, max_x, min_y, material))

    def hit(self, ray, t_min, t_max) -> HitRecord:
        return self.sides.hit(ray, t_min, t_max)
        