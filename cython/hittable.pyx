include "ray.pyx"

cdef class Material:

    def scatter(ray_in, hit_record):
        pass


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

    def set_face_normal(self, Ray ray, Vector normal) -> None:
        """set normal and save if hit from inside or outside"""
        if dot(ray.direction, normal) > 0.0:
            self.hit_from_outside = 1
        else:
            self.hit_from_outside = 0
            
        if self.hit_from_outside:
            self.normal = normal
        else:
            self.normal = normal*-1



cdef class Hittable:

    def hit(self, Ray ray, double t_min, double t_max) -> HitRecord:
        pass



cdef class Sphere(Hittable):
    """defines a sphere as a hittable"""

    cdef public Vector center
    cdef public double radius

    def hit(self, Ray ray, double t_min, double t_max) -> HitRecord:
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

        cdef HitRecord hit_record = HitRecord(hit_point, t_hit)
        hit_record.set_face_normal(ray, surface_normal)

        return hit_record

    def get_surface_normal(self, Vector surface_point) -> None:
        """get normal vector of a surface point (pointing outwards)"""
        return (surface_point - self.center) / self.radius


cdef class HittableList(Hittable):

    cdef list hittable_objects

    def __init__(self, Hittable hittable):
        self.hittable_objects = [hittable]

    def add(self, Hittable hittable) -> None:
        self.hittable_objects.append(hittable)

    def hit(self, Ray ray, double t_min, double t_max) -> HitRecord:
        """compute the closest hit to origin"""
        cdef HitRecord closest_hit = None
        cdef HitRecord hit_record

        for i in range(len(self.hittable_objects)):
            hittable = self.hittable_objects[i]
            hit_record = hittable.hit(ray, t_min, t_max)
            if hit_record is not None:
                if closest_hit is not None:
                    closest_hit = hit_record
                elif closest_hit.t > hit_record.t:
                    closest_hit = hit_record
        return closest_hit