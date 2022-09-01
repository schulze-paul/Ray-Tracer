from libcpp cimport bool


cdef class AxisAlignedBoundingBox(Hittable):
    cdef public Vector maximum
    cdef public Vector minimum

    def __init__(self, point_a: Vector, point_b: Vector):
        cdef double min_x = min(point_a.x(), point_b.x())
        cdef double min_y = min(point_a.y(), point_b.y())
        cdef double min_z = min(point_a.z(), point_b.z())
        
        cdef double max_x = max(point_a.x(), point_b.x())
        cdef double max_y = max(point_a.y(), point_b.y())
        cdef double max_z = max(point_a.z(), point_b.z())
        
        self.minimum = Vector(min_x, min_y, min_z)
        self.maximum = Vector(max_x, max_y, max_z)

    cpdef bool is_hit(self, Ray ray, double t_min, double t_max, double[:] t_hit):
        cdef Py_ssize_t a
        cdef double divisior
        cdef double t0
        cdef double t1

        for a in range(3):
            divisior = 1.0/ray.direction[a]

            t0 = (self.minimum[a] - ray.origin[a])*divisior
            t1 = (self.maximum[a] - ray.origin[a])*divisior

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

    cpdef AxisAlignedBoundingBox bounding_box(self, double time0, double time1):
        return self


cpdef AxisAlignedBoundingBox surrounding_box(AxisAlignedBoundingBox box_0, AxisAlignedBoundingBox box_1):
    cdef Vector small_point = Vector(min(box_0.minimum.x(), box_1.minimum.x()),
                                      min(box_0.minimum.y(), box_1.minimum.y()),
                                      min(box_0.minimum.z(), box_1.minimum.z()))
    cdef Vector large_point = Vector(max(box_0.maximum.x(), box_1.maximum.x()),
                                      max(box_0.maximum.y(), box_1.maximum.y()),
                                      max(box_0.maximum.z(), box_1.maximum.z()))

    return AxisAlignedBoundingBox(small_point, large_point)