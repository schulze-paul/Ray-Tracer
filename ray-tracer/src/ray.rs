use crate::vec3::Vec3;

#[derive(Debug, Clone, Copy)]
pub struct Ray {
    pub origin: Vec3,
    pub direction: Vec3,
    pub t: f64,
}

impl Ray {
    pub fn new(origin: Vec3, direction: Vec3) -> Ray {
        let normalized_direction = direction/direction.length();
        Ray {
            origin,
            direction: normalized_direction,
            t: 0.0
        }
    }
    pub fn at(&self, t: f64) -> Vec3{
        self.origin + t*self.direction
    }
}








