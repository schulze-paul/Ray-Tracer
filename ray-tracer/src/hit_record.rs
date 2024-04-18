use crate::{Vec3, Ray};
use crate::dot;

#[derive(Debug)]
pub struct HitRecord {
    pub is_hit: bool,
    pub t_hit: f64,
    pub normal: Vec3,
    pub hit_point: Vec3,
}

impl HitRecord {
    pub fn new() -> HitRecord {
        HitRecord{
            is_hit: false,
            t_hit: 0.0,
            normal: Vec3::zero(),
            hit_point: Vec3::zero()
        }
    }
    pub fn is_front_face(&self, ray: &Ray) -> bool {
        return dot(ray.direction, self.normal) < 0.0;
    }
}
