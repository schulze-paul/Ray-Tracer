use crate::{random_float, dot, Vec3, ONB, Hit};
use std::f64::consts;


pub trait PDF {
    fn generate(&self) -> Option<Vec3>;
    fn value(&self, direction: Vec3) -> f64;
}

pub struct CosinePDF {
    uvw: ONB,
}

impl CosinePDF {
    pub fn from(normal: Vec3) -> CosinePDF {
        return CosinePDF{
            uvw: ONB::from_w(normal),
        }
    }
    fn random_cosine_direction(&self) -> Vec3 {
        let r1 = random_float(0.0, 1.0);
        let r2 = random_float(0.0, 1.0);
        let z = (1.0 - r2).sqrt();

        let phi = 2.0*consts::PI * r1;
        let x = phi.cos() * r2.sqrt();
        let y = phi.sin() * r2.sqrt();
        return Vec3::new(x, y, z);
    }
}
impl PDF for CosinePDF {
    fn generate(&self) -> Option<Vec3> {
        return Some(self.uvw.local_from_vec(
            self.random_cosine_direction()));
    }
    fn value(&self, direction: Vec3) -> f64 {
        let cosine = dot(direction.unit(), self.uvw.w());
        return f64::max(cosine, 0.0);
    }
}

pub struct HittablePDF<'a> {
    origin: Vec3,
    objects: &'a dyn Hit,
}

impl HittablePDF<'_> {
    pub fn new(origin: Vec3, objects: & dyn Hit) -> HittablePDF<'_>{
        return HittablePDF {origin, objects};
    }
}

impl PDF for HittablePDF<'_> {
    fn generate(&self) -> Option<Vec3> {
        return self.objects.random_to_surface(self.origin);
    }
    fn value(&self, direction: Vec3) -> f64 {
        return self.objects.pdf_value(self.origin, direction);
    }
}
