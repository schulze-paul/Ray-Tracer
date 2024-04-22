use std::cmp::Ordering;

use rand::Rng;

use crate::{HitRecord, Material, Ray, Vec3, HitType};
use crate::dot;



#[derive(Debug, Clone)]
pub enum Hittable <'a>{
    HittableList(HittableListStruct<'a>),
    Sphere(SphereStruct<'a>),
    BoundingBox(BoundingBoxStruct),
    BHVNode(BVHNodeStruct<'a>),
}

#[derive(Debug, Clone)]
pub struct SphereStruct <'a>{
    pub center: Vec3, 
    pub radius: f64, 
    pub material: &'a Material,
}

#[derive(Debug, Clone, Copy)]
pub struct BoundingBoxStruct {
    pub min_corner: Vec3, 
    pub max_corner: Vec3,
}

pub trait Hit<'a> {
    fn hit(&'a self, ray: &'a Ray, range: [f64;2]) -> HitType;
}

pub trait MaterialTrait {
    fn material(&self) -> Option<&Material>;
}

pub trait BoundingVolumeTrait {
    fn bounding_volume(&self) -> Option<BoundingBoxStruct>;
}

impl <'a>Hit<'a> for Hittable<'a> {
    fn hit(&'a self, ray: &'a Ray, range: [f64;2]) -> HitType {
        match self {
            Hittable::Sphere(s) =>       s.hit(ray, range),
            Hittable::HittableList(l) => l.hit(ray, range),
            Hittable::BoundingBox(b) =>  b.hit(ray, range),
            Hittable::BHVNode(n) =>      n.hit(ray, range),
        }
    }
}

impl <'a>MaterialTrait for Hittable<'_> {
    fn material(&self) -> Option<&Material> {
        match self {
            Hittable::Sphere(s) =>       Some(&s.material),
            Hittable::HittableList(_) => None,
            Hittable::BoundingBox(_) =>  None,
            Hittable::BHVNode(_) =>      None,
        }
    }
}

impl BoundingVolumeTrait for Hittable<'_> {
    fn bounding_volume(&self) -> Option<BoundingBoxStruct> {
        match self {
            Hittable::Sphere(s) =>       Some(s.bounding_volume()),
            Hittable::HittableList(l) => l.bounding_volume(),
            Hittable::BoundingBox(b) =>  Some(b.bounding_volume()),
            Hittable::BHVNode(n) =>      Some(n.bounding_volume()),
        }
    }
}

#[derive(Debug, Clone)]
pub struct HittableListStruct <'a>{
    pub list: Vec<&'a Hittable<'a>>
}

impl <'a>HittableListStruct<'a> {
    pub fn new() -> HittableListStruct<'a> {
        return HittableListStruct{
            list: Vec::new() 
        }
    }
    pub fn from(list: Vec<&'a Hittable<'a>>) -> HittableListStruct<'a> {
        return HittableListStruct{list}
    }
    pub fn push(mut self, hittable: &'a Hittable<'a>) -> HittableListStruct<'a> {
        self.list.push(hittable);
        return self;
    }
    fn hit(&'a self, ray: &'a Ray, range: [f64;2]) -> HitType {
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

impl <'a>SphereStruct<'_> {
    pub fn new(center: Vec3, radius: f64, material: &Material) -> SphereStruct{
        SphereStruct{center, radius, material}
    }
    pub fn hit(&'a self, ray: &'a Ray, range: [f64;2]) -> HitType {
        let oc = ray.origin - self.center;              // origin to center
        let a = dot(ray.direction, ray.direction);      // direction squared
        let b = 2.0 * dot(oc, ray.direction);           // 2 * alignment of center direction and ray direction
        let c = dot(oc,oc) - self.radius * self.radius; // center distance squared - radius squared
        let discriminant = b * b - 4.0 * a * c;

        if discriminant < 0.0 {
            return HitType::None; // no hit
        }
        // ray in direction of sphere
        let mut hit_at_t = (-b - discriminant.sqrt()) / (2.0 * a);
        if !(hit_at_t < range[1] && hit_at_t > range[0]) {
            // not in range, try other hit
            hit_at_t = (-b + discriminant.sqrt()) / (2.0 * a);
            if !(hit_at_t < range[1] && hit_at_t > range[0])
            {
                // not in range, no hit
                return HitType::None;
            }
        }
        let normal = self.get_normal(ray.at(hit_at_t));
        let rec = HitRecord::new(hit_at_t ,ray, normal)
            .with_material(self.material);
        return HitType::Hit(rec);

    }
    fn get_normal(&self, point_on_surface: Vec3) -> Vec3 {
        (point_on_surface - self.center) / self.radius
    }
    pub fn bounding_volume(&self) -> BoundingBoxStruct {
        BoundingBoxStruct::new(
               self.center - self.radius*Vec3::ones(),
               self.center + self.radius*Vec3::ones()
        )
    }
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
    pub fn bounding_volume(self) -> BoundingBoxStruct {
        self
    }


    pub fn hit(self, ray: &Ray, range: [f64;2]) -> HitType {
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
}

#[derive(Debug, Clone)]
pub enum BVHNodeType<'a>{
    BVHNode(Box<BVHNodeStruct<'a>>),
    Hittable(&'a Hittable<'a>)
}

impl <'a>BVHNodeType<'_> {
    fn hit(&'a self, ray: &'a Ray, range: [f64;2]) -> HitType {
        match self {
            BVHNodeType::BVHNode(n) =>  n.hit(ray, range),
            BVHNodeType::Hittable(h) => h.hit(ray, range),
        }
    }
    pub fn bounding_volume(&self) -> BoundingBoxStruct {
        match self {
            BVHNodeType::BVHNode(n) =>  n.bounding_volume(),
            BVHNodeType::Hittable(h) => h.bounding_volume()
                .expect("BVHNode hittable has no bounging volume"),
        }
    }
}

#[derive(Debug, Clone)]
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

        objects.list[start..end].sort_by(|a, b| Self::is_closer(a, b, axis));
        let mid = start + object_span / 2;
        let left =  BVHNodeType::BVHNode( Box::new(BVHNodeStruct::new(objects, start, mid)));
        let right = BVHNodeType::BVHNode( Box::new(BVHNodeStruct::new(objects, mid, end)));
        return BVHNodeStruct {left, right};
    }

    pub fn is_closer(obj_a: &Hittable, obj_b: &Hittable, axis: usize) -> Ordering {
        match obj_a.bounding_volume().zip(obj_b.bounding_volume()) {
            None => panic!("No bounding box in bvhnode init"),
            Some((a, b)) => {
                return a.min_corner[axis].partial_cmp(&b.min_corner[axis]).expect("no ordering found");
            }
        }

    }
    fn hit(&'a self, ray: &'a Ray, range: [f64;2]) -> HitType {
        match self.bounding_volume().hit(ray, range) {
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
    pub fn bounding_volume(&self) -> BoundingBoxStruct {
        return BoundingBoxStruct::surrounding(
            self.left.bounding_volume(),
            self.right.bounding_volume(),
        )
    }
}
