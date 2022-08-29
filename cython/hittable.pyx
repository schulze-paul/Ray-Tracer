include "ray.pyx"

import cython

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
        if dot(ray.direction, normal) < 0.0:
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
    # defines a sphere as a hittable
    cdef public Vector center 
    cdef public double radius
    cdef public Material material

    def __init__(self, Vector center, double radius, Material material):
        self.center = center
        self.radius = radius
        self.material = material

    @cython.cdivision(True)
    def hit(self, Ray ray, double t_min, double t_max) -> HitRecord:
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

    def get_surface_normal(self, surface_point: Vector):
        # get normal vector of a surface point (pointing outwards)
        return (surface_point - self.center) / self.radius


cdef class HittableList(Hittable):

    cdef list hittable_objects

    def __init__(self, Hittable hittable):
        self.hittable_objects = []
        self.hittable_objects.apppend(hittable)

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






cdef class Lambertian(Material):

    cdef Color albedo

    def __init__(self, Color albedo):
        self.albedo = albedo

    def scatter(self, ray_in, hit_record):
        scatter_direction = hit_record.normal + random_unit_vector()
        if scatter_direction.near_zero():
            scatter_direction = hit_record.normal

        scattered = Ray(hit_record.hit_point, scatter_direction)

        return True, scattered, self.albedo


cdef class Metal(Material):
    
    cdef Color albedo
    cdef double fuzz

    def __init__(self, albedo: Color, fuzz: float):
        self.albedo = albedo
        if fuzz < 1:
            self.fuzz = fuzz
        else:
            self.fuzz = 1

    def scatter(self, ray_in, hit_record):
        vector_in = unit_vector(ray_in.direction)
        reflected = reflect(vector_in, hit_record.normal)
        scattered = Ray(hit_record.hit_point, reflected +
                        random_in_unit_sphere()*self.fuzz)
        is_scattered = dot(scattered.direction, hit_record.normal) > 0

        return is_scattered, scattered, self.albedo


cdef class Dielectric(Material):

    cdef double index_of_refraction

    def __init__(self, index_of_refraction):
        self.index_of_refraction = index_of_refraction

    def scatter(self, ray_in, hit_record):
        attenuation = Color(1, 1, 1)
        if hit_record.hit_from_outside == 1:
            refraction_ratio = 1/self.index_of_refraction
        else:
            refraction_ratio = self.index_of_refraction

        unit_direction = unit_vector(ray_in.direction)
        cos_theta = fmin(dot(unit_direction*-1, hit_record.normal), 1.0)
        sin_theta = sqrt(1.0-cos_theta**2)
        cannot_refract = refraction_ratio*sin_theta > 1
        reflectance = self.__reflectance(cos_theta, refraction_ratio)
        if cannot_refract or (reflectance > random.random()):
            direction = reflect(unit_direction, hit_record.normal)
        else:
            direction = refract(
                unit_direction, hit_record.normal, refraction_ratio)

        scattered = Ray(hit_record.hit_point, direction)

        return True, scattered, attenuation

    def __reflectance(self, cos_theta: float, refraction_ratio: float) -> float:
        # Schlick's approximation
        r0 = (1-refraction_ratio) / (1+refraction_ratio)
        r0 = r0**2
        return r0 * (1-r0)*(1-cos_theta)**5
