include "ray.pyx"

cdef class HitRecord:
    cdef public Vector hit_point
    cdef public double t
    cdef public int hit_from_outside
    cdef public Vector normal


    def void set_face_normal(self, Ray ray, Vector normal):
        """set normal and save if hit from inside or outside"""
        self.hit_from_outside = dot(ray.direction, normal)
        if self.hit_from_outside:
            self.normal = normal
        else:
            self.normal = normal*-1



cdef class Hittable:

    def HitRecord hit(self, Ray ray, double t_min, double t_max):
        pass



cdef class Sphere(Hittable):
    """defines a sphere as a hittable"""

    cdef public Vector center
    cdef public double radius

    def HitRecord hit(self, Ray ray, double t_min, double t_max):
        """
        computes hit event with a ray

        """
        cdef Vector origin_to_center = ray.get_origin() - self.center
        cdef double a = dot(ray.direction, ray.direction)
        cdef double half_b = dot(origin_to_center, ray.direction)
        cdef double c = dot(origin_to_center, origin_to_center) - self.radius*self.radius
        cdef double discriminant = half_b*half_b - a*c

        if discriminant < 0:
            # no hit
            return None

        # hit registered, is hit in range?
        cdef double sqrt_discriminant = math.sqrt(discriminant)
        cdef double t_hit = -(half_b + sqrt_discriminant) / a

        if t_hit < t_min or t_hit > t_max:
            -(half_b - sqrt_discriminant) / a
            if t_hit < t_min or t_hit > t_max:
                # outside of range
                return None

        # collect data about hit:
        cdef Vector hit_point = ray(t_hit)
        cdef Vector surface_normal = self.get_surface_normal(hit_point)

        HitRecord hit_record = HitRecord(hit_point, t_hit)
        hit_record.set_face_normal(ray, surface_normal)

        return hit_record

    def void get_surface_normal(self, Vector surface_point):
        """get normal vector of a surface point (pointing outwards)"""
        return (surface_point - self.center) / self.radius


class HittableList(Hittable):

    cdef list hittable_objects = []

    def __init__(self, Hittable hittable):
        self.hittable_objects = [hittable]

    def void add(self, Hittable hittable) -> None:
        self.hittable_objects.append(hittable)

    def HitRecord hit(self, *args, **kwargs):
        """compute the closest hit to origin"""
        closest_hit = None

        for hittable in self.hittable_objects:
            hit_record = hittable.hit(*args, **kwargs)
            if hit_record is not None:
                if closest_hit is None:
                    closest_hit = hit_record
                elif closest_hit.t > hit_record.t:
                    closest_hit = hit_record
        return closest_hit