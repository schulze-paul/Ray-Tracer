use crate::{Color, Ray, Scatter, HitRecord, ScatterRecord};


#[derive(Debug, Clone, Copy)]
pub struct Emissive {
    color: Color
}
impl Emissive {
    pub fn new(color: Color) -> Emissive {
        Emissive{color}
    }
}
impl Scatter for Emissive {
    fn scatter<'a>(&'a self, ray: &Ray, hit_record: &'a HitRecord) -> ScatterRecord {
        let scattered = ScatterRecord::new(hit_record);
        return scattered
    }
    fn attenuation(&self) -> Color {
        return Color::black();
    }
    fn emittance(&self) -> Color {
        return self.color;
    }
}
