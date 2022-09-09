from copy import copy
from dataclasses import dataclass
import math
import random
import numpy as np

# local imports
from cython_extension import Vector, HitRecord, Hittable,  RectangleXY, RectangleYZ, RectangleZX
from cython_extension import Material
from cython_extension import AxisAlignedBoundingBox, surrounding_box


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

    def bounding_box(self, time0: float, time1: float) -> AxisAlignedBoundingBox:
        if len(self.hittable_objects):
            return None

        first_box = True
        for hittable_object in self.hittable_objects:
            temp_box = hittable_object.bounding_box(time0, time1)
            if temp_box is None:
                return None
            else:
                if first_box:
                    output_box = temp_box
                else:
                    output_box = surrounding_box(output_box, temp_box)
                first_box = False

        return output_box


class Box(Hittable):
    """Rectangular cuboid."""

    def __init__(self, point0: Vector, point1: Vector, material: Material) -> None:
        min_x, max_x = sorted((point0.data[0], point1.data[0]))
        min_y, max_y = sorted((point0.data[1], point1.data[1]))
        min_z, max_z = sorted((point0.data[2], point1.data[2]))

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

    def bounding_box(self, time0: float, time1: float):
        return AxisAlignedBoundingBox(self.minimum_point, self.maximum_point)
