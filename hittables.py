from dataclasses import dataclass
from abc import ABC, abstractmethod
import math
import random
import numpy as np

# local imports
from hittable import Ray
from hittable import Vector, Color, dot, unit_vector, random_unit_vector, reflect, refract, random_in_unit_sphere
# , Lambertian, Dielectric, Metal
from hittable import HitRecord, Hittable, Material, Sphere  # , HittableList

"""
class Material(ABC):

    @abstractmethod
    def scatter(ray_in, hit_record) -> tuple:
        pass


@dataclass
class HitRecord():
    hit_point: Vector
    t: float
    material: Material

    def set_face_normal(self, ray: Ray, normal: Vector) -> None:
        # set normal and save if hit from inside or outside
        self.hit_from_outside = dot(ray.direction, normal) < 0
        if self.hit_from_outside:
            self.normal = normal
        else:
            self.normal = -normal


class Hittable(ABC):
    # Base class for all hitables

    @abstractmethod
    def hit(self, ray: Ray, t_min: float, t_max: float) -> HitRecord:
        pass



@dataclass
class Sphere(Hittable):
    # defines a sphere as a hittable

    center: Vector
    radius: float
    material: Material

    def hit(self, ray: Ray, t_min: float, t_max: float) -> HitRecord:
        # computes hit event with a ray

        origin_to_center = ray.get_origin() - self.center
        a = dot(ray.direction, ray.direction)
        half_b = dot(origin_to_center, ray.direction)
        c = dot(origin_to_center, origin_to_center) - self.radius**2
        discriminant = half_b*half_b - a*c

        if discriminant < 0:
            # no hit
            return None

        # hit registered, is hit in range?
        sqrt_discriminant = math.sqrt(discriminant)
        t_hit = -(half_b + sqrt_discriminant) / a

        if t_hit < t_min or t_hit > t_max:
            -(half_b - sqrt_discriminant) / a
            if t_hit < t_min or t_hit > t_max:
                # outside of range
                return None

        # collect data about hit:
        hit_point = ray(t_hit)
        surface_normal = self.get_surface_normal(hit_point)

        hit_record = HitRecord(hit_point, t_hit, self.material)
        hit_record.set_face_normal(ray, surface_normal)

        return hit_record

    def get_surface_normal(self, surface_point: Vector):
        get normal vector of a surface point (pointing outwards)
        return (surface_point - self.center) / self.radius

"""


class HittableList(Hittable):
    def __init__(self, hittable: Hittable) -> None:
        self.hittable_objects = [hittable]

    def add(self, hittable: Hittable) -> None:
        self.hittable_objects.append(hittable)

    def hit(self, ray, t_min, t_max) -> HitRecord:
        # compute the closest hit to origin
        closest_t_hit = None
        closest_index = None
        t_hit = np.empty(1)

        for hittable_index, hittable in enumerate(self.hittable_objects):
            if hittable.is_hit(ray, t_min, t_max, t_hit):
                if closest_t_hit is None:
                    closest_t_hit = t_hit[0]
                    closest_index = hittable_index
                elif closest_t_hit > t_hit[0]:
                    closest_t_hit = t_hit[0]
                    closest_index = hittable_index
        if closest_index is None:
            return None
        else:
            return self.hittable_objects[closest_index].get_hit_record(ray, closest_t_hit)


@dataclass
class Lambertian(Material):

    albedo: Color

    def scatter(self, ray_in, hit_record):
        scatter_direction = hit_record.normal + random_unit_vector()
        if scatter_direction.near_zero():
            scatter_direction = hit_record.normal

        scattered = Ray(hit_record.hit_point, scatter_direction)

        return True, scattered, self.albedo


class Metal(Material):
    def __init__(self, albedo: Color, fuzz: float):
        self.albedo = albedo
        if fuzz < 1:
            self.fuzz = fuzz
        else:
            self.fuzz = 1

    def scatter(self, ray_in, hit_record):
        vector_in = unit_vector(ray_in.direction)
        reflected = reflect(vector_in, hit_record.normal)
        scattered = Ray(hit_record.hit_point, reflected +
                        random_in_unit_sphere()*self.fuzz)
        is_scattered = dot(scattered.direction, hit_record.normal) > 0

        return is_scattered, scattered, self.albedo


@dataclass
class Dielectric(Material):
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
            direction = refract(
                unit_direction, hit_record.normal, refraction_ratio)

        scattered = Ray(hit_record.hit_point, direction)

        return True, scattered, attenuation

    def __reflectance(self, cos_theta: float, refraction_ratio: float) -> float:
        # Schlick's approximation
        r0 = (1-refraction_ratio) / (1+refraction_ratio)
        r0 = r0**2
        return r0 * (1-r0)*(1-cos_theta)**5
