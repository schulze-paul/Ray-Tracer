from copy import copy

from .hittables import HittableList
from cython_extension import surrounding_box


class BoundingVolumeHierarchyNode():

    def __init__(self, hittable_objects: HittableList, time0, time1, start=0, end=None, ) -> None:

        if end == None:
            end = len(hittable_objects)-1

        hittable_objects = copy(hittable_objects)

        self.axis = 2  # random.randint(0, 2)

        object_span = end+1 - start

        if object_span == 1:
            self.left = hittable_objects[start]
            self.right = self.left
        elif object_span == 2:
            if self.__box_min(hittable_objects[start]) < self.__box_min(hittable_objects[end]):
                self.left = hittable_objects[start]
                self.right = hittable_objects[start+1]
            else:
                self.left = hittable_objects[start+1]
                self.right = hittable_objects[start]
        else:
            hittable_objects = sorted(
                hittable_objects, key=self.__box_min)

            mid = start + object_span//2
            self.left = BoundingVolumeHierarchyNode(
                hittable_objects, time0, time1, start, mid)
            self.right = BoundingVolumeHierarchyNode(
                hittable_objects, time0, time1, mid, end)

        box_left = self.left.bounding_box(time0, time1)
        box_right = self.right.bounding_box(time0, time1)

        if box_left is None or box_right is None:
            raise ValueError(
                "No bounding box in Bounding Volume Hierarchy None constructor.")

        self.box = surrounding_box(box_left, box_right)

    def hit(self, ray, t_min, t_max):
        if self.box.hit(ray, t_min, t_max) is None:
            return None

        hit_left = self.left.hit(ray, t_min, t_max)
        if hit_left is None:
            hit_right = self.right.hit(ray, t_min, t_max)
        else:
            hit_right = self.right.hit(ray, t_min, t_max)

        if hit_left is not None and hit_right is not None:
            if hit_left.t < hit_right.t:
                return hit_left
            else:
                return hit_right
        elif hit_left is not None:
            return hit_left
        elif hit_right is not None:
            return hit_right
        else:
            return None

    def bounding_box(self, time0, time1):
        return self.box

    def __box_min(self, hittable):
        box = hittable.bounding_box(0, 0)

        if box is None:
            raise ValueError(
                "No bounding box in Bounding Volume Hierarchy None constructor.")
        return box.minimum.data[self.axis]
