use crate::{Vec3, Color, Ray, HitRecord, ScatterRecord};
use crate::material::Scatter;

#[derive(Debug, Clone, Copy)]
pub struct Lambertian {
    color: Color
}

impl Lambertian {
    pub fn new(color: Color) -> Lambertian {
        Lambertian{color}
    }
}
impl Scatter for Lambertian {
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
    fn emittance(&self) -> Color {
        return Color::black();
    }
}

