use crate::{Vec3, Color, Ray, HitRecord};
use crate::{dot, random_float};

pub enum Material {
    Metal(MetalStruct),
    Dielectric(DielectricStruct),
    Lambertian(LambertianStruct)
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
    fn scatter(&self, ray: &Ray, hit_record: &HitRecord, attenuation: &mut Color) -> Ray;
}

impl Scatter for Material {
    fn scatter(&self, ray: &Ray, hit_record: &HitRecord, attenuation: &mut Color) -> Ray {
        match self {
            Material::Metal(m) =>      m.scatter(ray, hit_record, attenuation),
            Material::Dielectric(d) => d.scatter(ray, hit_record, attenuation),
            Material::Lambertian(l) => l.scatter(ray, hit_record, attenuation),
        }
    }
}

impl MetalStruct {
    pub fn new(albedo: Color, fuzz: f64) -> MetalStruct {
        MetalStruct{albedo, fuzz}
    }
    fn scatter(&self, ray: &Ray, hit_record: &HitRecord, attenuation: &mut Color) -> Ray {
        *attenuation = self.albedo.clone();
        return Ray::new(
            // move away from hit point to avoid double hit
            hit_record.hit_point + hit_record.normal/1000.0, 
            reflect(ray.direction, hit_record.normal))
    }
}

fn reflect(v: Vec3, n: Vec3) -> Vec3 {
    return v - 2.0 * dot(v, n) * n;
}

impl DielectricStruct {
    pub fn new(refractive_index: f64) -> DielectricStruct {
        DielectricStruct{refractive_index}
    }
    fn scatter(&self, ray: &Ray, hit_record: &HitRecord, attenuation: &mut Color) -> Ray {
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
        let mut scatter_origin: Vec3;
        let mut scatter_direction: Vec3;
        if cannot_refract || reflectance(cos_theta, refraction_ratio) > random_float(0.0, 1.0) {
            scatter_origin = hit_record.hit_point + unit_normal/1024.0;
            scatter_direction = reflect(ray.direction, unit_normal);
        } else {
            scatter_origin = hit_record.hit_point - unit_normal/1024.0;
            scatter_direction = refract(ray.direction, unit_normal, refraction_ratio);
        }
        return Ray::new(
            scatter_origin, // avoid double hit 
            scatter_direction);
        
    }
}
fn refract(v_in: Vec3, normal: Vec3, refraction_ratio: f64) -> Vec3 {
    let cos_theta = f64::min(dot(-v_in, normal), 1.0);
    let out_perpendicular = refraction_ratio * (v_in + cos_theta*normal);
    let out_parallel = -(1.0 - out_perpendicular.length_squared()).abs().sqrt() * normal;
    return out_perpendicular + out_parallel;

}
fn reflectance(cosine: f64, ref_idx: f64) -> f64 {
    // Use Schlick's approximation for reflectance.
    let r0 = (
            (1.0 - ref_idx) / (1.0 + ref_idx)
        ).powi(2);
    return r0 + (1.0 - r0) * (1.0 - cosine).powi(5);
}



pub struct LambertianStruct {
    color: Color
}

impl LambertianStruct {
    pub fn new(color: Color) -> LambertianStruct {
        LambertianStruct{color}
    }
    fn scatter(&self, ray: &Ray, hit_record: &HitRecord, attenuation: &mut Color) -> Ray {
        *attenuation = self.color.clone();
        let scatter_direction = hit_record.normal + Vec3::random_in_unit_sphere().unit();
        let scatter_origin = hit_record.hit_point + hit_record.normal/1024.0;
        return Ray::new(scatter_origin, scatter_direction);
    }
}


