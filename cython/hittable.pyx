include "ray.pyx"

import cython

cdef class Material:
    """
    Base class for all materials.
    
    All sub-classes need to implement the scatter method.
    The emittance is 0 by default.
    """
    def scatter(self, ray_in, hit_record):
        """Compute if scatttering took place, get outcoming ray and attenuation."""
        pass

    cpdef emitted(self):
        """Get color value of emmited light of material (if applicable)."""
        return Color(0,0,0) 


cdef class HitRecord:
    """Class to store data from a collision of a ray with a hittable."""
    cdef public Vector hit_point        # the coordinates of the point where the collision took place.
    cdef public double t                # the t value of the inco,ing ray at the hit location.
    cdef public Material material       # the material of the hittable that was hit.
    cdef public int hit_from_outside    # save if hit came from inside or outside of the hittable
    cdef public Vector normal           # normal vector of the surface at the collision point
                                        # (points inside if the incoming ray came from inside the hittable volume).

    def __init__(self, Vector hit_point, double t, Material material):
        self.hit_point = hit_point
        self.t = t
        self.material = material

    cpdef void set_face_normal(self, Ray ray, Vector normal):
        """Set normal and save if hit from inside or outside"""
        if dot(ray.direction, normal) < 0.0:
            self.hit_from_outside = 1
        else:
            self.hit_from_outside = 0
            
        if self.hit_from_outside:
            # normal pointing outside
            self.normal = normal
        else:
            # normal pointing inside
            self.normal = normal*-1



cdef class Hittable:
    """
    Base class for all hittable objects.
    
    All sub-classes need to implement the hit function 
    """
    cpdef HitRecord hit(self, Ray ray, double t_min, double t_max):
        """Computes hit event with a ray (if ray intersects with the hittable)"""
        return None



cdef class Sphere(Hittable):
    """Defines a sphere as a hittable geometry."""
    cdef public Vector center 
    cdef public double radius
    cdef public Material material

    def __init__(self, Vector center, double radius, Material material):
        self.center = center
        self.radius = radius
        self.material = material

    @cython.cdivision(True)
    cpdef HitRecord hit(self, Ray ray, double t_min, double t_max):      
        # set up equations to check if hit is possible and where
        cdef Vector origin_to_center = ray.origin - self.center
        cdef double a = dot(ray.direction, ray.direction)
        cdef double half_b = dot(origin_to_center, ray.direction)
        cdef double c = dot(origin_to_center, origin_to_center) - self.radius**2
        cdef double discriminant = half_b*half_b - a*c
        
        if discriminant < 0:
            return None # hit not possible
        
        # compute t of ray at hit
        cdef double sqrt_discriminant = sqrt(discriminant)
        cdef double t_hit = -(half_b + sqrt_discriminant) / a

        # is hit in range set by user?
        if t_hit < t_min or t_hit > t_max:
            -(half_b - sqrt_discriminant) / a
            if t_hit < t_min or t_hit > t_max:
                return None # outside of range

        # collect data about hit:
        cdef Vector hit_point = ray(t_hit)
        cdef Vector surface_normal = self.get_surface_normal(hit_point)

        cdef HitRecord hit_record = HitRecord(hit_point, t_hit, self.material)
        hit_record.set_face_normal(ray, surface_normal)

        return hit_record

    cpdef Vector get_surface_normal(self, surface_point: Vector):
        """get normal vector of a surface point (pointing outwards)"""
        return (surface_point - self.center) / self.radius


cdef class MovableSphere(Hittable):    
    """Defines a moving as a hittable geometry."""

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
        """Get the center point of the sphere depending on the time"""
        cdef double time_fraction = (time-self.time0)/(self.time1-self.time0)
        return self.center0 + (self.center1 - self.center0) * time_fraction

    @cython.cdivision(True)
    cpdef HitRecord hit(self, Ray ray, double t_min, double t_max):
        # set up equations to check if hit is possible and where
        cdef Vector origin_to_center = ray.origin - self.center(ray.time)
        cdef double a = dot(ray.direction, ray.direction)
        cdef double half_b = dot(origin_to_center, ray.direction)
        cdef double c = dot(origin_to_center, origin_to_center) - self.radius**2
        cdef double discriminant = half_b*half_b - a*c

        if discriminant < 0:
            return None # hit not possible

        # compute t of ray at hit
        cdef double sqrt_discriminant = sqrt(discriminant)
        cdef double t_hit = -(half_b + sqrt_discriminant) / a

        # is hit in range set by user?
        if t_hit < t_min or t_hit > t_max:
            -(half_b - sqrt_discriminant) / a
            if t_hit < t_min or t_hit > t_max:
                return None # outside of range

        # collect data about hit:
        cdef Vector hit_point = ray(t_hit)
        cdef Vector surface_normal = self.get_surface_normal(hit_point, ray.time)

        cdef HitRecord hit_record = HitRecord(hit_point, t_hit, self.material)
        hit_record.set_face_normal(ray, surface_normal)

        return hit_record

    cpdef Vector get_surface_normal(self, surface_point: Vector, double time):
        """get normal vector of a surface point (pointing outwards)"""
        return (surface_point - self.center(time)) / self.radius
