
from math import inf
import numpy as np
import random
from python.hittables import HittableList
from sphere import AxisAlignedBoundingBox, Hittable, Ray, Vector, surrounding_box, HitRecord


class BoundingVolumeHierarchyNode(Hittable):

    def __init__(self, object_list: HittableList, time0: float, time1: float, start: int = -1, end: int = -1):

        if start == -1:
            start = 0
        if end == -1:
            end = len(object_list.hittable_objects)

        axis = random_int(0, 2)

        def sort_key(box):
            return get_box_min(box, axis)

        objects_span = end - start

        if objects_span == 1:
            self.left = object_list.hittable_objects[start]
            self.right = self.left

        elif objects_span == 2:
            if sort_key(object_list.hittable_objects[start]) < sort_key(object_list.hittable_objects[start+1]):
                self.left = object_list.hittable_objects[start]
                self.right = object_list.hittable_objects[start+1]
            else:
                self.left = object_list.hittable_objects[start+1]
                self.right = object_list.hittable_objects[start]

        else:
            sorted_list = sorted(object_list.hittable_objects, key=sort_key)
            object_list = HittableList(sorted_list[0])

            for i in range(1, len(sorted_list)):
                object_list.add(sorted_list[i])

            mid = start + objects_span//2
            self.left = BoundingVolumeHierarchyNode(
                object_list, time0, time1, start, mid)
            self.right = BoundingVolumeHierarchyNode(
                object_list, time0, time1, mid, end)

        box_left = self.left.bounding_box(time0, time1)
        box_right = self.right.bounding_box(time0, time1)
        if box_left == False or box_right == False:
            box_left = self.left.box
            box_right = self.right.box

        self.box = surrounding_box(box_left, box_right)

    def is_hit(self, ray: Ray, t_min: float, t_max: float, t_hit):
        if not self.box.is_hit(ray, t_min, t_max, t_hit):
            return False

        t_hit_left = np.zeros(1)
        hit_left = self.left.is_hit(ray, t_min, t_max, t_hit_left)
        t_hit_right = np.zeros(1)
        hit_right = self.right.is_hit(ray, t_min, t_max, t_hit_right)

        if hit_left and not hit_right:
            t_hit[0] = t_hit_left[0]
        elif hit_right and not hit_left:
            t_hit[0] = t_hit_right[0]
        elif hit_left and hit_right:
            if t_hit_left < t_hit_right:
                t_hit[0] = t_hit_left[0]
            else:
                t_hit[0] = t_hit_right[0]

        return hit_left or hit_right

    def get_hit_record(self, ray: Ray, t_hit: float) -> HitRecord:
        t_hit_left = np.zeros(1)
        hit_left = self.left.is_hit(ray, 0, inf, t_hit_left)
        t_hit_right = np.zeros(1)
        hit_right = self.right.is_hit(ray, 0, inf, t_hit_right)

        if hit_left and not hit_right:
            return self.left.get_hit_record(ray, t_hit_left[0])
        elif hit_right and not hit_left:
            return self.right.get_hit_record(ray, t_hit_right[0])
        elif hit_left and hit_right:
            if t_hit_left < t_hit_right:
                return self.left.get_hit_record(ray, t_hit_left[0])
            else:
                return self.right.get_hit_record(ray, t_hit_right[0])

        return None


def get_box_min(hittable: Hittable, axis: int) -> float:
    if hittable.has_box():
        return hittable.bounding_box(0, 0).minimum[axis]


def random_int(min: int, max: int):
    return int(random.random()*(max-min) - min)
