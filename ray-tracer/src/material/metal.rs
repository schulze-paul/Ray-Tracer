
use crate::{Color, Ray, HitRecord, ScatterRecord};
use crate::material::Scatter;
use crate::material::reflect;

#[derive(Debug)]
pub struct Metal {
    albedo: Color,
    fuzz: f64
}
impl Metal {
    pub fn new(albedo: Color, fuzz: f64) -> Metal {
        Metal{albedo, fuzz}
    }
}
impl Scatter for Metal {
    fn scatter<'a>(&'a self, ray: &Ray, hit_record: &'a HitRecord) -> ScatterRecord {
        ScatterRecord::new(hit_record)
            .push(1.0, reflect(ray.direction, hit_record.normal)
        )
    }
    fn attenuation(&self) -> Color {
        return self.albedo;
    }
    fn emittance(&self) -> Color {
        return Color::black();
    }
}
