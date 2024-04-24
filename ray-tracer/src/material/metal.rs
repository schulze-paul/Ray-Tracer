
use crate::{Color, Ray, HitRecord, ScatterRecord};
use crate::material::Scatter;
use crate::material::reflect;

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
