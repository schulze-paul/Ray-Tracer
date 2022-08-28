from abc import ABC, abstractmethod

class Material(ABC):
    
    @abstractmethod
    def scatter(ray_in: Ray: hit_record: HitRecord) -> tuple:
        pass
        
@dataclass   
class Lambertian():
        
        albedo: Color
        
        def scatter(ray_in, hit_record):
            scatter_direction = hit_record.normal +random_unit_vector()
            if scatter_direction.near_zero():
                scatter_direction = hit_record.normal
                
            scattered = Ray(record.hit_point, scatter_direction)

            return True, scattered, self.albedo
        