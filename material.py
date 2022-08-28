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