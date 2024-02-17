use crate::Vec3;

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
            normal: Vec3::new(0.0,0.0,0.0),
            hit_point: Vec3::new(0.0,0.0,0.0)
        }
    }
}
