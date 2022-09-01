include "ray.pyx"
include "aabb.pyx"

import cython
from libcpp cimport bool

cdef class Material:

    def scatter(ray_in, hit_record):
        pass

    cpdef Color emitted(self, Vector point):
        return Color(0,0,0)

cdef class HitRecord:
    cdef public Vector hit_point
    cdef public double t
    cdef public Material material
    cdef public int hit_from_outside
    cdef public Vector normal

    def __init__(self, Vector hit_point, double t, Material material):
        self.hit_point = hit_point
        self.t = t
        self.material = material

    cpdef void set_face_normal(self, Ray ray, Vector normal):
        """set normal and save if hit from inside or outside"""
        if dot(ray.direction, normal) < 0.0:
            self.hit_from_outside = 1
        else:
            self.hit_from_outside = 0
            
        if self.hit_from_outside:
            self.normal = normal
        else:
            self.normal = normal*-1



cdef class Hittable:

    cpdef bool is_hit(self, Ray ray, double t_min, double t_max, double[:] t_hit):
        return False
    
    cpdef bool has_box(self):
        return False

    cpdef AxisAlignedBoundingBox bounding_box(self, double time0, double time1):
        return False

