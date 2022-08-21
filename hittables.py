from dataclasses import dataclass
from abc import ABC, abstractmethod
import math

# local imports
from ray import Ray
from vector_utils import Vector
from vector_cython import dot


@dataclass
class HitRecord():
    hit_point: Vector
    t: float

    def set_face_normal(self, ray: Ray, normal: Vector) -> None:
        """set normal and save if hit from inside or outside"""
        self.hit_from_outside = dot(ray.direction, normal)
        if self.hit_from_outside:
            self.normal = normal
        else:
            self.normal = -normal


class Hittable(ABC):
    """Base class for all hitables"""

    @abstractmethod
    def hit(self, ray: Ray, t_min: float, t_max: float) -> HitRecord:
        pass


@dataclass
class Sphere(Hittable):
    """defines a sphere as a hittable"""

    center: Vector
    radius: float

    def hit(self, ray: Ray, t_min: float, t_max: float) -> HitRecord:
        """
        computes hit event with a ray

        """
        origin_to_center = ray.origin - self.center
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

        hit_record = HitRecord(hit_point, t_hit)
        hit_record.set_face_normal(ray, surface_normal)

        return hit_record

    def get_surface_normal(self, surface_point: Vector):
        """get normal vector of a surface point (pointing outwards)"""
        return (surface_point - self.center) / self.radius


class HittableList(Hittable):
    def __init__(self, hittable: Hittable) -> None:
        self.hittable_objects = [hittable]

    def add(self, hittable: Hittable) -> None:
        self.hittable_objects.append(hittable)

    def hit(self, *args, **kwargs) -> HitRecord:
        """compute the closest hit to origin"""
        closest_hit = None

        for hittable in self.hittable_objects:
            hit_record = hittable.hit(*args, **kwargs)
            if hit_record is not None:
                if closest_hit is None:
                    closest_hit = hit_record
                elif closest_hit.t > hit_record.t:
                    closest_hit = hit_record
        return closest_hit
