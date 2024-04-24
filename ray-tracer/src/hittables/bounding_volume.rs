use std::cmp::Ordering;
use rand::Rng;

use crate::{Vec3, Ray, Hit, HitType, HittableListStruct}; 

#[derive(Debug, Clone, Copy)]
pub struct BoundingBoxStruct {
    pub min_corner: Vec3, 
    pub max_corner: Vec3,
}
impl BoundingBoxStruct {
    pub fn new(corner_a: Vec3, corner_b: Vec3) -> BoundingBoxStruct {
        let min_corner = Vec3::new(
            f64::min(corner_a.x(), corner_b.x()),
            f64::min(corner_a.y(), corner_b.y()),
            f64::min(corner_a.z(), corner_b.z()),
        );
        let max_corner = Vec3::new(
            f64::max(corner_a.x(), corner_b.x()),
            f64::max(corner_a.y(), corner_b.y()),
            f64::max(corner_a.z(), corner_b.z()),
        );
        BoundingBoxStruct{min_corner, max_corner}
    }
    pub fn surrounding(box_a: BoundingBoxStruct, box_b: BoundingBoxStruct) -> BoundingBoxStruct{
        let min_corner = Vec3::new(
            f64::min(box_a.min_corner.x(), box_b.min_corner.x()),
            f64::min(box_a.min_corner.y(), box_b.min_corner.y()),
            f64::min(box_a.min_corner.z(), box_b.min_corner.z()),
        );
        let max_corner = Vec3::new(
            f64::max(box_a.max_corner.x(), box_b.max_corner.x()),
            f64::max(box_a.max_corner.y(), box_b.max_corner.y()),
            f64::max(box_a.max_corner.z(), box_b.max_corner.z()),
        );
        BoundingBoxStruct{min_corner, max_corner}
    }
}
impl Hit for BoundingBoxStruct {
     fn hit(&self, ray: &Ray, range: [f64;2]) -> HitType {
        for dim in 0..3 {
            let inv_d = 1.0/ray.direction[dim];
            let mut t0 = (self.min_corner[dim] - ray.origin[dim]) * inv_d;
            let mut t1 = (self.max_corner[dim] - ray.origin[dim]) * inv_d;
            if inv_d.is_sign_negative() {
                std::mem::swap(&mut t0, &mut t1);

            }
            let t_min = 
                if t0 > range[0] {t0} else {range[0]};
            let t_max = 
                if t1 < range[0] {t1} else {range[0]};
            if t_max <= t_min{
                return HitType::BoundingHit
            }
        }
        return HitType::None
    }
    fn bounding_volume(&self) -> Option<BoundingBoxStruct> {
        Some(*self)
    }
}


#[derive(Clone)]
pub enum BVHNodeType<'a>{
    BVHNode(Box<BVHNodeStruct<'a>>),
    Hittable(&'a dyn Hit)
}

impl <'a>BVHNodeType<'a> {
    fn hit(&self, ray: &Ray, range: [f64;2]) -> HitType {
        match self {
            BVHNodeType::BVHNode(n) =>  n.hit(ray, range),
            BVHNodeType::Hittable(h) => h.hit(ray, range),
        }
    }
    pub fn bounding_volume(&self) -> BoundingBoxStruct {
        match self {
            BVHNodeType::BVHNode(n) =>  n.bounding_volume()
                .expect("BVHNode hittable has no bounging volume"),
            BVHNodeType::Hittable(h) => h.bounding_volume()
                .expect("BVHNode hittable has no bounging volume"),
        }
    }
}

#[derive(Clone)]
pub struct BVHNodeStruct<'a> {
    left:  BVHNodeType<'a>,
    right: BVHNodeType<'a>,
}

impl <'a>BVHNodeStruct<'_> {
    pub fn new(objects: &mut HittableListStruct<'a>, start: usize, end: usize) -> BVHNodeStruct<'a> {
        let mut rng = rand::thread_rng();
        let axis: usize = rng.gen_range(0..3);
        let object_span = end - start;
        if object_span == 1 {
            return BVHNodeStruct{
                left: BVHNodeType::Hittable(objects.list[start]),
                right: BVHNodeType::Hittable(objects.list[start]),
            }
        }
        if object_span == 2 {
            if Self::is_closer(objects.list[start], objects.list[start+1], axis).is_lt() {
                return BVHNodeStruct{
                    left: BVHNodeType::Hittable(objects.list[start]),
                    right: BVHNodeType::Hittable(objects.list[start+1]),
                }
            } else {
                return BVHNodeStruct{
                    left: BVHNodeType::Hittable(objects.list[start+1]),
                    right: BVHNodeType::Hittable(objects.list[start]),
                }
            }
        }

        objects.list[start..end].sort_by(|a, b| Self::is_closer(*a, *b, axis));
        let mid = start + object_span / 2;
        let left =  BVHNodeType::BVHNode( Box::new(BVHNodeStruct::new(objects, start, mid)));
        let right = BVHNodeType::BVHNode( Box::new(BVHNodeStruct::new(objects, mid, end)));
        return BVHNodeStruct {left, right};
    }

    pub fn is_closer(obj_a: &'a dyn Hit, obj_b: &dyn Hit, axis: usize) -> Ordering {
        match obj_a.bounding_volume().zip(obj_b.bounding_volume()) {
            None => panic!("No bounding box in bvhnode init"),
            Some((a, b)) => {
                return a.min_corner[axis].partial_cmp(&b.min_corner[axis]).expect("no ordering found");
            }
        }

    }
}
impl<'a> Hit for BVHNodeStruct<'a> {
    fn hit(&self, ray: &Ray, range: [f64;2]) -> HitType {
        match self.bounding_volume().expect("bvh node has no bounding volume")
                .hit(ray, range) {
            HitType::None => return HitType::None,
            _ => {
                let left_hit =  self.left.hit(ray, range);
                let right_hit = self.right.hit(ray, range);
                match left_hit {
                    HitType::None => {
                        match right_hit {
                            HitType::Hit(h) =>      return HitType::Hit(h),
                            HitType::BoundingHit => return HitType::BoundingHit,
                            HitType::None =>        return HitType::None,
                        }
                    },
                    HitType::BoundingHit => {
                        match right_hit {
                            HitType::Hit(h) => return HitType::Hit(h),
                            _ =>               return HitType::BoundingHit,
                        }
                    },
                    HitType::Hit(lh) => {
                        match right_hit {
                            HitType::Hit(rh) => {
                                if lh.t_hit < rh.t_hit {
                                    return HitType::Hit(lh)
                                } else {
                                    return HitType::Hit(rh)
                                }
                            },
                            _ => return HitType::Hit(lh),
                        }
                    },
                }
            }
        }


    }
    fn bounding_volume(&self) -> Option<BoundingBoxStruct> {
        return Some(BoundingBoxStruct::surrounding(
            self.left.bounding_volume(),
            self.right.bounding_volume(),
        ))
    }
}

