
use crate::{Ray, Hit, HitType, BoundingBoxStruct};

#[derive(Clone)]
pub struct HittableListStruct <'a>{
    pub list: Vec<&'a dyn Hit>
}

impl <'a>HittableListStruct<'a> {
    pub fn new() -> HittableListStruct<'a> {
        return HittableListStruct{
            list: Vec::new() 
        }
    }
    pub fn from(list: Vec<&'a dyn Hit>) -> HittableListStruct<'a> {
        return HittableListStruct{list}
    }
    pub fn push(mut self, hittable: &'a dyn Hit) -> HittableListStruct<'a> {
        self.list.push(hittable);
        return self;
    }
}
impl<'a> Hit for HittableListStruct<'a>{
    fn hit(&self, ray: &Ray, range: [f64;2]) -> HitType {
        let mut closest_hit_record = HitType::None;
        for hittable in &self.list {
            let hit_record = hittable.hit(ray, range);
            match hit_record {
                HitType::Hit(h) => {
                    match closest_hit_record {
                        HitType::Hit(ref c) => {
                            if h.t_hit < c.t_hit {
                                closest_hit_record = HitType::Hit(h) 
                            }
                        }
                        _ => closest_hit_record = HitType::Hit(h),
                    }
                }
                _ => continue,
            }
        }
        return closest_hit_record;
    }
    fn bounding_volume(&self) -> Option<BoundingBoxStruct> {
        if self.list.len() == 0 {
            return None;
        }
        let mut bbox: BoundingBoxStruct = self.list[0].bounding_volume()?;

        for object in &self.list {
            bbox = BoundingBoxStruct::surrounding(
                bbox, 
                object.bounding_volume()?
            )
        }
        return Some(bbox);
    }
}
