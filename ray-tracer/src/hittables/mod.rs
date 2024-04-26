
pub mod sphere;
pub mod cuboid;
pub mod hittable_list;
pub mod bounding_volume;

use core::f64;

use crate::{hit_record::HitRecord, vec3::Vec3, Ray};
use bounding_volume::BoundingBox;

pub trait Hit {
    fn hit(&self, ray: &Ray, range: Interval) -> Option<HitRecord>;
    fn bounding_volume(&self) -> BoundingBox;
    fn random_to_surface(&self, origin: Vec3) -> Option<Vec3>;
    fn pdf_value(&self, origin: Vec3, direction: Vec3) -> f64;
}

#[derive(Copy, Clone, Debug)]
pub struct Interval {
    min: f64,
    max: f64,
}

impl Interval {
    pub fn new(min: f64, max: f64) -> Interval {
        return Interval{
            min: f64::min(min, max), 
            max: f64::max(min, max), 
        }
    }
    pub fn contains(&self, num: f64) -> bool {
        return num > self.min && num < self.max
    }
    pub fn outer(range_a: Interval, range_b: Interval) -> Interval{
        return Interval::new(
            f64::min(range_a.min, range_b.min),
            f64::max(range_a.max, range_b.max),
        ) 
    }
    pub fn inner(range_a: Interval, range_b: Interval) -> Interval{
        return Interval::new(
            f64::max(range_a.min, range_b.min),
            f64::min(range_a.max, range_b.max),
        ) 
    }
    pub fn do_overlap(range_a: Interval, range_b: Interval) -> bool {
        return range_a.contains(range_b.min) || range_a.contains(range_b.max)
    }
}

