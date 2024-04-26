
use crate::{Vec3, HitRecord, random_usize, BoundingBox, Hit, Interval, Ray};

#[derive(Clone)]
pub struct HittableList <'a>{
    pub list: Vec<&'a dyn Hit>
}

impl <'a>HittableList<'a> {
    pub fn new() -> HittableList<'a> {
        return HittableList{
            list: Vec::new() 
        }
    }
    pub fn from(list: Vec<&'a dyn Hit>) -> HittableList<'a> {
        return HittableList{list}
    }
    pub fn push(mut self, hittable: &'a dyn Hit) -> HittableList<'a> {
        self.list.push(hittable);
        return self;
    }
}
impl<'a> Hit for HittableList<'a>{
    fn hit(&self, ray: &Ray, range: Interval) -> Option<HitRecord> {
        let mut closest_hit_record: Option<HitRecord> = None;
        for hittable in &self.list {
            let hit_record = hittable.hit(ray, range);
            match (closest_hit_record, hit_record) {
                (Some(ch), Some(h)) => {
                    if h.t_hit < ch.t_hit {
                        closest_hit_record = Some(h) 
                    }
                }
                (Some(_), None) => continue,
                _ => closest_hit_record = hit_record
            }
        }
        return closest_hit_record;
    }
    fn bounding_volume(&self) -> BoundingBox {
        if self.list.len() == 0 {
            return BoundingBox::new(Vec3::zero(), Vec3::zero());
        }
        let mut bbox: BoundingBox = self.list[0].bounding_volume();

        for object in &self.list {
            bbox = BoundingBox::surrounding(
                bbox, 
                object.bounding_volume()
            )
        }
        return bbox;
    }
    fn pdf_value(&self, origin: crate::vec3::Vec3, direction: crate::vec3::Vec3) -> f64 {
        let weight = 1.0/(self.list.len() as f64);
        return self.list.iter().map(|h| weight * h.pdf_value(origin, direction)).sum();
    }
    fn random_to_surface(&self, origin: crate::vec3::Vec3) -> Option<Vec3> {
        let random_index = random_usize(0, self.list.len());
        return self.list[random_index].random_to_surface(origin);
    }
}
