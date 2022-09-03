include "ray.pyx"

import cython

cdef class Material:

    def scatter(self, ray_in, hit_record):
        pass

    cpdef emitted(self):
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

    cpdef HitRecord hit(self, Ray ray, double t_min, double t_max):
        return None



cdef class Sphere(Hittable):
    # defines a sphere as a hittable
    cdef public Vector center 
    cdef public double radius
    cdef public Material material

    def __init__(self, Vector center, double radius, Material material):
        self.center = center
        self.radius = radius
        self.material = material

    @cython.cdivision(True)
    cpdef HitRecord hit(self, Ray ray, double t_min, double t_max):
        # computes hit event with a ray

        cdef Vector origin_to_center = ray.origin - self.center
        cdef double a = dot(ray.direction, ray.direction)
        cdef double half_b = dot(origin_to_center, ray.direction)
        cdef double c = dot(origin_to_center, origin_to_center) - self.radius**2
        cdef double discriminant = half_b*half_b - a*c

        if discriminant < 0:
            # no hit
            return None

        # hit registered, is hit in range?
        cdef double sqrt_discriminant = sqrt(discriminant)
        cdef double t_hit = -(half_b + sqrt_discriminant) / a

        if t_hit < t_min or t_hit > t_max:
            -(half_b - sqrt_discriminant) / a
            if t_hit < t_min or t_hit > t_max:
                # outside of range
                return None

        # collect data about hit:
        cdef Vector hit_point = ray(t_hit)
        cdef Vector surface_normal = self.get_surface_normal(hit_point)

        cdef HitRecord hit_record = HitRecord(hit_point, t_hit, self.material)
        hit_record.set_face_normal(ray, surface_normal)

        return hit_record

    cpdef Vector get_surface_normal(self, surface_point: Vector):
        # get normal vector of a surface point (pointing outwards)
        return (surface_point - self.center) / self.radius


cdef class MovableSphere(Hittable):
    # defines a sphere as a hittable
    cdef public Vector center0 
    cdef public Vector center1
    cdef public double time0
    cdef public double time1
    cdef public double radius
    cdef public Material material

    def __init__(self, Vector center0, Vector center1, double time0, double time1, double radius, Material material):
        self.center0 = center0
        self.center1 = center1
        self.time0 = time0
        self.time1 = time1
        self.radius = radius
        self.material = material

    cpdef Vector center(self, double time):
        cdef double time_fraction = (time-self.time0)/(self.time1-self.time0)
        return self.center0 + (self.center1 - self.center0) * time_fraction

    @cython.cdivision(True)
    cpdef HitRecord hit(self, Ray ray, double t_min, double t_max):
        # computes hit event with a ray

        cdef Vector origin_to_center = ray.origin - self.center(ray.time)
        cdef double a = dot(ray.direction, ray.direction)
        cdef double half_b = dot(origin_to_center, ray.direction)
        cdef double c = dot(origin_to_center, origin_to_center) - self.radius**2
        cdef double discriminant = half_b*half_b - a*c

        if discriminant < 0:
            # no hit
            return None

        # hit registered, is hit in range?
        cdef double sqrt_discriminant = sqrt(discriminant)
        cdef double t_hit = -(half_b + sqrt_discriminant) / a

        if t_hit < t_min or t_hit > t_max:
            -(half_b - sqrt_discriminant) / a
            if t_hit < t_min or t_hit > t_max:
                # outside of range
                return None

        # collect data about hit:
        cdef Vector hit_point = ray(t_hit)
        cdef Vector surface_normal = self.get_surface_normal(hit_point, ray.time)

        cdef HitRecord hit_record = HitRecord(hit_point, t_hit, self.material)
        hit_record.set_face_normal(ray, surface_normal)

        return hit_record

    cpdef Vector get_surface_normal(self, surface_point: Vector, double time):
        # get normal vector of a surface point (pointing outwards)
        return (surface_point - self.center(time)) / self.radius
