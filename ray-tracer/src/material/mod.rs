use crate::{Vec3, Color, Ray, HitRecord, ScatterRecord};
use crate::dot;

pub mod metal;
pub mod dielectric;
pub mod lambertian;

pub trait Scatter {
    fn scatter<'a>(&'a self, ray: &Ray, hit_record: &'a HitRecord) -> ScatterRecord;
    fn attenuation(&self) -> Color;
}



fn reflect(v: Vec3, n: Vec3) -> Vec3 {
    return v - 2.0 * dot(v, n) * n;
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




