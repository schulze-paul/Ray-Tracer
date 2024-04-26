use std::cmp::Ordering;
use rand::Rng;

use crate::{hit_record::HitRecord, Hit, HittableList, Interval, Ray, Vec3}; 

#[derive(Debug, Clone, Copy)]
pub struct BoundingBox {
    ranges: [Interval; 3],
}
impl BoundingBox {
    pub fn new(corner_a: Vec3, corner_b: Vec3) -> BoundingBox {
        BoundingBox{ranges: [
            Interval::new(corner_a.x(), corner_b.x()),
            Interval::new(corner_a.y(), corner_b.y()),
            Interval::new(corner_a.z(), corner_b.z()),
        ]}
    }
    pub fn surrounding(box_a: BoundingBox, box_b: BoundingBox) -> BoundingBox{
        BoundingBox{ranges: [
            Interval::outer(box_a.x(), box_b.x()),
            Interval::outer(box_a.y(), box_b.y()),
            Interval::outer(box_a.z(), box_b.z()),
        ]}
    }
    fn hit(&self, ray: &Ray, range: Interval) -> bool {
        for dim in 0..3 {
            let inv_d = 1.0/ray.direction[dim];
            let dim_range = Interval::new(
                (self.ranges[dim].min - ray.origin[dim]) * inv_d,
                (self.ranges[dim].max - ray.origin[dim]) * inv_d,
            );
            if Interval::do_overlap(range, dim_range) {
                return true;
            }
        }
        return false;
    }
    pub fn x(&self) -> Interval {
        return self.ranges[0];
    }
    pub fn y(&self) -> Interval {
        return self.ranges[1];
    }
    pub fn z(&self) -> Interval {
        return self.ranges[2];
    }
}


#[derive(Clone)]
pub enum BVHNodeType<'a>{
    BVHNode(Box<BVHNode<'a>>),
    Hittable(&'a dyn Hit)
}

impl <'a>BVHNodeType<'a> {
    fn hit(&self, ray: &Ray, range: Interval) -> Option<HitRecord> {
        match self {
            BVHNodeType::BVHNode(n) =>  n.hit(ray, range),
            BVHNodeType::Hittable(h) => h.hit(ray, range),
        }
    }
    pub fn bounding_volume(&self) -> BoundingBox {
        match self {
            BVHNodeType::BVHNode(n) =>  n.bounding_volume(),
            BVHNodeType::Hittable(h) => h.bounding_volume(),
        }
    }
}
#[derive(Clone)]
pub struct BVHNode<'a> {
    left:  BVHNodeType<'a>,
    right: BVHNodeType<'a>,
}

impl <'a>BVHNode<'_> {
    pub fn new(objects: &mut HittableList<'a>, start: usize, end: usize) -> BVHNode<'a> {
        let mut rng = rand::thread_rng();
        let axis: usize = rng.gen_range(0..3);
        let object_span = end - start;
        if object_span == 1 {
            return Self::from_hittables(objects, start, start)
        }
        if object_span == 2 {
            if Self::is_closer(objects.list[start], objects.list[start+1], axis).is_lt() {
                return Self::from_hittables(objects, start, start+1)
            } else {
                return Self::from_hittables(objects, start+1, start)
            }
        }

        objects.list[start..end].sort_by(|a, b| Self::is_closer(*a, *b, axis));
        return Self::split(objects, start, end)
    }

    fn is_closer(obj_a: &'a dyn Hit, obj_b: &dyn Hit, axis: usize) -> Ordering {
        return obj_a.bounding_volume().ranges[axis].min.total_cmp(&obj_b.bounding_volume().ranges[axis].min);
    }
    fn from_hittables(objects: &mut HittableList<'a>, index_left: usize, index_right: usize) -> BVHNode<'a> {
            return BVHNode{
                left: BVHNodeType::Hittable(objects.list[index_left]),
                right: BVHNodeType::Hittable(objects.list[index_right]),
            }
    }
    fn split(objects: &mut HittableList<'a>, start: usize, end: usize) -> BVHNode<'a> {
        let mid = start + (end - start) / 2;
        let left =  BVHNodeType::BVHNode(Box::new(BVHNode::new(objects, start, mid)));
        let right =  BVHNodeType::BVHNode(Box::new(BVHNode::new(objects, mid, end)));
        return BVHNode {left, right};
    }
}
impl<'a> Hit for BVHNode<'a> {
    fn hit(&self, ray: &Ray, range: Interval) -> Option<HitRecord> {
        if !self.bounding_volume()
            .hit(ray, range) {
            return None;
         }
        let left_hit =  self.left.hit(ray, range);
        let right_hit = self.right.hit(ray, range);
        return match (left_hit.clone(), right_hit.clone()) {
            (None, None) =>         None,
            (Some(_), None) =>      left_hit,
            (None, Some(_)) =>      right_hit,
            (Some(lh), Some(rh)) => {
                if lh.is_closer_than(rh) {
                    left_hit
                } else {
                    right_hit
                }
            },
            
        }
    }
    fn bounding_volume(&self) -> BoundingBox {
        return BoundingBox::surrounding(
            self.left.bounding_volume(),
            self.right.bounding_volume(),
        )
    }
    fn pdf_value(&self, origin: Vec3, direction: Vec3) -> f64 {
        return 0.0;
    }
    fn random_to_surface(&self, origin: Vec3) -> Option<Vec3> {
        return None;
    }
}

