
pub mod sphere;
pub mod cuboid;
pub mod hittable_list;
pub mod bounding_volume;


use crate::{Ray, HitType};
use bounding_volume::BoundingBoxStruct;

pub trait Hit {
    fn hit(&self, ray: &Ray, range: [f64;2]) -> HitType;
    fn bounding_volume(&self) -> Option<BoundingBoxStruct>;
}


