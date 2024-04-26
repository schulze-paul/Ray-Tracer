use crate::{Color, Ray, HitRecord, ScatterRecord};
use crate::dot;
use crate::material::Scatter;
use crate::material::{reflectance, reflect, refract};

#[derive(Debug, Clone, Copy)]
pub struct Dielectric {
    refractive_index: f64
}
impl Dielectric {
    pub fn new(refractive_index: f64) -> Dielectric {
        Dielectric{refractive_index}
    }
}
impl Scatter for Dielectric {
    fn scatter<'a>(&'a self, ray: &Ray, hit_record: &'a HitRecord) -> ScatterRecord {
        let mut refraction_ratio = self.refractive_index;
        let mut unit_normal = -hit_record.normal;
        if hit_record.is_front_face(ray) {
            refraction_ratio = 1.0/refraction_ratio;
            unit_normal = -unit_normal;
        }
        let cos_theta = f64::min(dot(-ray.direction, unit_normal), 1.0);
        let sin_theta = (1.0 - cos_theta * cos_theta).sqrt();
        let cannot_refract = refraction_ratio * sin_theta > 1.0;

        let scattered = ScatterRecord::new(hit_record);
        let reflected_direction = reflect(ray.direction, unit_normal);
        if cannot_refract {
            return scattered.push(1.0, reflected_direction);
        }
        let reflectance = reflectance(cos_theta, refraction_ratio);
        let refracted_direction = refract(ray.direction, unit_normal, refraction_ratio);
        return scattered
            .push(reflectance, reflected_direction)
            .push(1.0-reflectance, refracted_direction);
    }
    fn attenuation(&self) -> Color {
        return Color::white();
    }
    fn emittance(&self) -> Color {
        return Color::black();
    }
}
