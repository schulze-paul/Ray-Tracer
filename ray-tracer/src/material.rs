use crate::{Vec3, Color, Ray, HitRecord, ScatterRecord};
use crate::{dot, random_float};


pub trait Scatter {
    fn scatter<'a>(&'a self, ray: &Ray, hit_record: &'a HitRecord) -> ScatterRecord;
    fn attenuation(&self) -> Color;
}


#[derive(Debug)]
pub struct MetalStruct {
    albedo: Color,
    fuzz: f64
}
impl MetalStruct {
    pub fn new(albedo: Color, fuzz: f64) -> MetalStruct {
        MetalStruct{albedo, fuzz}
    }
}
impl Scatter for MetalStruct {
    fn scatter<'a>(&'a self, ray: &Ray, hit_record: &'a HitRecord) -> ScatterRecord {
        ScatterRecord::new(hit_record)
            .push(1.0, reflect(ray.direction, hit_record.normal)
        )
    }
    fn attenuation(&self) -> Color {
        return self.albedo;
    }
}

fn reflect(v: Vec3, n: Vec3) -> Vec3 {
    return v - 2.0 * dot(v, n) * n;
}

#[derive(Debug)]
pub struct DielectricStruct {
    refractive_index: f64
}
impl DielectricStruct {
    pub fn new(refractive_index: f64) -> DielectricStruct {
        DielectricStruct{refractive_index}
    }
}
impl Scatter for DielectricStruct {
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



#[derive(Debug)]
pub struct LambertianStruct {
    color: Color
}

impl LambertianStruct {
    pub fn new(color: Color) -> LambertianStruct {
        LambertianStruct{color}
    }
}
impl Scatter for LambertianStruct {
    fn scatter<'a>(&'a self, _ray: &Ray, hit_record: &'a HitRecord) -> ScatterRecord {

        let mut scattered = ScatterRecord::new(hit_record);
        let num_scattered = 1;
        for _ in 0..num_scattered {
            scattered = scattered.push(
                1.0/(num_scattered as f64),
                hit_record.normal + Vec3::random_in_unit_sphere().unit()
            );
        }
        return scattered;
    }
    fn attenuation(&self) -> Color {
        return self.color;
    }
}


