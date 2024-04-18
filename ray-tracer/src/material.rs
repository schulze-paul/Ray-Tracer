use rand::Rng;

use crate::{Vec3, Color, Ray, HitRecord};
use crate::{dot};

pub enum Material {
    Metal(MetalStruct),
    Dielectric(DielectricStruct)
}

#[derive(Debug)]
pub struct MetalStruct {
    albedo: Color,
    fuzz: f64
}

#[derive(Debug)]
pub struct DielectricStruct {
    refractive_index: f64
}

pub trait Scatter {
    fn scatter(&self, ray: &Ray, hit_record: &HitRecord) -> Ray;
}

impl Scatter for Material {
    fn scatter(&self, ray: &Ray, hit_record: &HitRecord) -> Ray {
        match self {
            Material::Metal(m) => m.scatter(ray, hit_record),
            Material::Dielectric(d) => d.scatter(ray, hit_record)
        }
    }
}

impl MetalStruct {
    pub fn new(albedo: Color, fuzz: f64) -> MetalStruct {
        MetalStruct{albedo, fuzz}
    }
    fn scatter(&self, ray: &Ray, hit_record: &HitRecord) -> Ray {
        return Ray::new(
            hit_record.hit_point,
            reflect(ray.direction, hit_record.normal))
    }
}

fn reflect(v_in: Vec3, normal: Vec3) -> Vec3 {
    v_in - 2.0*dot(v_in, normal) * normal
}

impl DielectricStruct {
    pub fn new(refractive_index: f64) -> DielectricStruct {
        DielectricStruct{refractive_index}
    }
    fn scatter(&self, ray: &Ray, hit_record: &HitRecord) -> Ray {
        let refraction_ratio = if hit_record.is_front_face(ray) {
            1.0/self.refractive_index
        } else {
            self.refractive_index
        };

        let unit_normal = if hit_record.is_front_face(ray) {
            hit_record.normal
        } else {
            -hit_record.normal
        };
        let cos_theta = f64::min(dot(-ray.direction, unit_normal), 1.0);
        let sin_theta = (1.0 - cos_theta * cos_theta).sqrt();

        let cannot_refract = refraction_ratio * sin_theta > 1.0;
        
        let scatter_direction: Vec3;
        if cannot_refract || reflectance(cos_theta, refraction_ratio) > random_float() {
            scatter_direction = reflect(ray.direction, unit_normal);
        } else {
            scatter_direction = refract(ray.direction, unit_normal, refraction_ratio);
        }
        return Ray::new(hit_record.hit_point, scatter_direction);
        
    }
}
fn refract(v_in: Vec3, normal: Vec3, refraction_ratio: f64) -> Vec3 {
    let cos_theta = dot(-v_in, normal).min(1.0);
    let out_perpendiculat = refraction_ratio * (v_in + cos_theta*normal);
    let out_parallel = -(1.0 - out_perpendiculat.length_squared()).abs().sqrt() * normal;
    return out_perpendiculat + out_parallel;

}
fn reflectance(cosine: f64, ref_idx: f64) -> f64 {
    // Use Schlick's approximation for reflectance.
    let r0 = ((1.0 - ref_idx) / (1.0 + ref_idx)).powi(2);
    return r0 + (1.0 - r0) * (1.0 - cosine).powi(5);
}

fn random_float() -> f64 {
    let mut rng = rand::thread_rng();
    return rng.gen()
}
