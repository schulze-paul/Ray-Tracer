# cython: profile=True

include "ray.pyx"


cdef class AxisAlignedBoundingBox:
    cdef public Vector maximum
    cdef public Vector minimum

    def __init__(self, point_a: Vector, point_b: Vector):
        cdef double min_x = min(point_a.data[0], point_b.data[0])
        cdef double min_y = min(point_a.data[1], point_b.data[1])
        cdef double min_z = min(point_a.data[2], point_b.data[2])
        
        cdef double max_x = max(point_a.data[0], point_b.data[0])
        cdef double max_y = max(point_a.data[1], point_b.data[1])
        cdef double max_z = max(point_a.data[2], point_b.data[2])
        
        self.minimum = Vector(min_x, min_y, min_z)
        self.maximum = Vector(max_x, max_y, max_z)

    cpdef HitRecord hit(self, Ray ray, double t_min, double t_max):
        cdef Py_ssize_t a
        cdef double divisior
        cdef double t0
        cdef double t1

        for a in range(3):
            divisior = 1.0/ray.direction.data[a]

            t0 = (self.minimum.data[a] - ray.origin.data[a])*divisior
            t1 = (self.maximum.data[a] - ray.origin.data[a])*divisior

            if divisior < 0:
                temp = t0
                t0 = t1
                t1 = temp

            if t0 > t_min:
                t_min = t0
            if t1 < t_max:
                t_max = t1

            if t_max <= t_min:
                return None

        return HitRecord(None,0,None)

    cpdef AxisAlignedBoundingBox bounding_box(self, double time0, double time1):
        return self


cpdef AxisAlignedBoundingBox surrounding_box(AxisAlignedBoundingBox box_0, AxisAlignedBoundingBox box_1):
    cdef Vector small_point = Vector(min(box_0.minimum.data[0], box_1.minimum.data[0]),
                                      min(box_0.minimum.data[1], box_1.minimum.data[1]),
                                      min(box_0.minimum.data[2], box_1.minimum.data[2]))
    cdef Vector large_point = Vector(max(box_0.maximum.data[0], box_1.maximum.data[0]),
                                      max(box_0.maximum.data[1], box_1.maximum.data[1]),
                                      max(box_0.maximum.data[2], box_1.maximum.data[2]))

    return AxisAlignedBoundingBox(small_point, large_point)
