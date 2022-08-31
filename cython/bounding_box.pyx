from http.client import PRECONDITION_FAILED
from hittable import Hittable, Vector, Ray


cdef class AxisAlignedBoundingBox(Hittable):

    def __init__(self, point_a: Vector, point_b: Vector):
        self.maximum = point_a
        self.minimum = point_b

    cpdef int is_hit(self, Ray ray, double t_min, double t_max, double[:] t_hit):
        cdef Py_ssize_t a
        cdef double divisior
        cdef double t0
        cdef double t1

        for a in range(3):
            divisior = 1/ray.direction[a]
            t0 = min((self.minimum[a] - ray.origin[a])/divisior,
                     (self.maximum[a] - ray.origin[a])/divisior,)
            t1 = max((self.minimum[a] - ray.origin[a])/divisior,
                     (self.maximum[a] - ray.origin[a])/divisior,)

            if divisior < 0:
                temp = t0
                t0 = t1
                t1 = temp

            if t0 > t_min:
                t_min = t0
            if t1 < t_max:
                t_max = t1

            if t_max <= t_min:
                return False

        return True

cpdef AxisAlignedBoundingBox surrounding_box(AxisAlignedBoundingBox box_0, AxisAlignedBoundingBox box_1):
    cdef Vector small_point = Vector(fmin(box_0.min().x(), box_1.min().x()),
                                      fmin(box_0.min().y(), box_1.min().y()),
                                      fmin(box_0.min().z(), box_1.min().z()))
    cdef Vector large_point = Vector(fmin(box_0.max().x(), box_1.max().x()),
                                      fmin(box_0.max().y(), box_1.max().y()),
                                      fmin(box_0.max().z(), box_1.max().z()))

    return AxisAlignedBoundingBox(small_point, large_point)