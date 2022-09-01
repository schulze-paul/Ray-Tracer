import random
include "sphere.pyx"

cdef class BoundingVolumeHierarchyNode(Hittable):
    cdef Hittable left
    cdef Hittable right
    cdef AxisAlignedBoundingBox box

    def __init__(self, HittableList object_list, double time0, double time1, int start=-1, int end=-1):
        
        if start == -1:
            start = 0
        if end == -1:
            end = len(object_list.hittable_objects)

        cdef int axis = random_int(0,2)
        def sort_key(box):
            return get_box_min(box, axis)


        cdef int objects_span = end - start

        if objects_span == 1:
            self.left = object_list.hittable_objects[start]
            self.right = self.left
        
        elif objects_span == 2:
            if object_list.hittable_objects[start].min()[axis] < object_list.hittable_objects[start+1].min()[axis]:
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
            self.left = BoundingVolumeHierarchyNode(object_list, start, mid, time0, time1)
            self.right = BoundingVolumeHierarchyNode(object_list, mid, end, time0, time1)

    cpdef int is_hit(self, Ray ray, double t_min, double t_max, double[:] t_hit):
        if not self.box.is_hit(ray, t_min, t_max, t_hit):
            return False 

        cdef double[:] t_hit_left 
        cdef int hit_left = self.left.is_hit(ray, t_min, t_max, t_hit_left)
        cdef double[:] t_hit_right 
        cdef int hit_right = self.right.is_hit(ray, t_min, t_max, t_hit_right)

        return hit_left or hit_right


def get_box_min(hitable: Hittable, axis: int) -> float:
    cdef AxisAlignedBoundingBox box = AxisAlignedBoundingBox(Vector(0,0,0), Vector(0,0,0))
    if hitable.bounding_box(0,0,box):
        return box.minimum[axis]

cdef int random_int(int min, int max):
    return int(random.random()*(max-min) - min)