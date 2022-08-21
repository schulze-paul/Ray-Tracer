from dataclasses import dataclass
from pickletools import read_unicodestring1
import numpy as np
from abc import ABC, abstractmethod
# local imports
from ray import Ray


@dataclass
class HitRecord():
    hit_point: np.array
    t: float

    def set_face_normal(self, ray: Ray, normal: np.ndarray) -> None:
        """set normal and save if hit from inside or outside"""
        self.hit_from_outside = np.dot(ray.direction, normal)
        if self.hit_from_outside:
            self.normal = normal
        else:
            self.normal = -normal


class Hittable(ABC):
    """Base class for all hitables"""

    @abstractmethod
    def hit(self, ray: Ray, t_min: float, t_max: float, hit_record: HitRecord) -> bool:
        pass


@dataclass
class Sphere(Hittable):
    """defines a sphere as a hittable"""

    center: np.ndarray
    radius: float

    def hit(self, ray: Ray, t_min: float, t_max: float) -> HitRecord:
        """
        computes hit event with a ray

        """
        origin_to_center = ray.origin - self.center
        a = np.dot(ray.direction, ray.direction)
        half_b = np.dot(origin_to_center, ray.direction)
        c = np.dot(origin_to_center, origin_to_center) - self.radius**2
        discriminant = half_b*half_b - a*c

        if discriminant > 0:
            # no hit
            return None

        # hit registered, is hit in range?
        sqrt_discriminant = np.sqrt(discriminant)
        t_hit = -(half_b + sqrt_discriminant) / a

        if t_hit < t_min or t_hit > t_max:
            -(half_b - sqrt_discriminant) / a
            if t_hit < t_min or t_hit > t_max:
                # outside of range
                return None

        # collect data about hit:
        hit_point = ray(t_hit)
        surface_normal = self.get_surface_normal(hit_point)

        hit_record = HitRecord(t_hit, hit_point)
        hit_record.set_face_normal(ray, surface_normal)

        return hit_record

    def get_surface_normal(self, surface_point: np.ndarray):
        """get normal vector of a surface point (pointing outwards)"""
        return (surface_point - self.center) / self.radius


def hit_sphere(center: np.ndarray, radius: float, ray: Ray) -> float:
    """
    computes if the given ray hits the sphere:

    the result depends on the root of this equation
    t^2*b*b + 2*t*b*(A-C) + (A-C)*(A-C)-r^2=0
    """

    origin_to_center = ray.origin - center
    a = np.dot(ray.direction, ray.direction)
    half_b = np.dot(origin_to_center, ray.direction)
    c = np.dot(origin_to_center, origin_to_center) - radius**2
    discriminant = half_b*half_b - a*c

    if discriminant < 0:
        # no solution
        return -1
    else:
        # get t of closest hit point
        return -(half_b + np.sqrt(discriminant)) / a
