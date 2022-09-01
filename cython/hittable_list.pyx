
from numpy.math cimport INFINITY

cdef class HittableList(Hittable):
    cdef public list hittable_objects

    def __init__(self, hittable: Hittable) -> None:
        self.hittable_objects = [hittable]

    def add(self, hittable: Hittable) -> None:
        self.hittable_objects.append(hittable)

    cpdef bool is_hit(self, Ray ray, double t_min, double t_max, double[:] t_hit):
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
            return False
        else:
            return True
    
    cpdef HitRecord get_hit_record(self, Ray ray, double t_hit):
        closest_t_hit = None
        closest_index = None
        t_hit_hittable = np.empty(1)

        for hittable_index, hittable in enumerate(self.hittable_objects):
            if hittable.is_hit(ray, 0, INFINITY, t_hit_hittable):
                if closest_t_hit is None:
                    closest_t_hit = t_hit_hittable[0]
                    closest_index = hittable_index
                elif closest_t_hit > t_hit_hittable[0]:
                    closest_t_hit = t_hit_hittable[0]
                    closest_index = hittable_index
        if closest_index is None:
            return None
        else:
            return self.hittable_objects[closest_index].get_hit_record(ray, closest_t_hit)

    cpdef bool has_box(self):
        if len(self.hittable_objects) == 0:
            return False

    cpdef AxisAlignedBoundingBox bounding_box(self, double time0, double time1):
        
        cdef AxisAlignedBoundingBox temp_box = AxisAlignedBoundingBox()
        cdef int first_box = True

        for hittable in self.hittable_objects:
            if hittable.bounding_box(time0, time1, temp_box):
                return False
            if first_box:
                output_box = temp_box
            else:
                output_box = surrounding_box(output_box, temp_box)
            first_box = False

        return output_box