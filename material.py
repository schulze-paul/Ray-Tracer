from abc import ABC, abstractmethod

class Material(ABC):
    
    @abstractmethod
    def scatter(ray_in: Ray: hit_record: HitRecord) -> tuple:
        pass
        
@dataclass   
class Lambertian(Material):
        
        albedo: Color
        
        def scatter(ray_in, hit_record):
            scatter_direction = hit_record.normal +random_unit_vector()
            if scatter_direction.near_zero():
                scatter_direction = hit_record.normal
                
            scattered = Ray(record.hit_point, scatter_direction)

            return True, scattered, self.albedo


class Metal(Material):
    def __init__(albedo: Color, fuzz: float):
        self.albedo=albedo
        if fuzz < 1:
            self.fuzz=fuzz
        else:
            self.fuzz=1
    
    def scatter(ray_in, hit_record):
        vector_in = unit_vector(ray_in.direction)
        reflected = reflect(vector_in, hit_record.normal)
        scattered = Ray(hit_record.hit_point, reflected + fuzz*random_in_unit_sphere())
        is_scattered = dot(scattered.direction, hit_record.normal) > 0
        
        return is_scattered, scattered, self.albedo
        
@dataclass
class Dielectric(Material):
    index_of_refraction: float
    
    def scatter(ray_in, hit_record):
        attenuation = Color(1,1,1)
        if hit_record.hit_from_outside:
            refraction_ratio = 1/self.index_of_refraction
        else:
            refraction_ratio = self.index_of_refraction
        
        unit_direction = unit_vector(ray_in.direction)
        cos_theta = min(dot(-unit_direction, hit_record.normal), 1.0)
        sin_theta = math.sqrt(1.0-cos_theta**2)
        cannot_refract = refraction_ratio*sin_theta > 1
        reflectance = self.__reflectance(cos_theta, refraction_ratio)
        if cannot_refract or reflectance > random.random():
            direction = reflect(unit_direction, hit_record.normal)
        else:
            direction = refract(unit_direction, hit_record.normal, refraction_ratio)
        
        scattered = Ray(hit_record.hit_point, direction)
        
        return True, scattered, attenuation
        
    def __reflectance(cos_theta: float, refraction_ratio: float) -> float:
        # Schlick's approximation
        r0 = (1-refraction_ratio) / (1+refraction_ratio)
        r0=r0**2
        return r0 *(1-r0)*(1-cos_theta)**5