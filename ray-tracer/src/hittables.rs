use std::cmp::Ordering;
use std::fmt::write;

use rand::Rng;

use crate::{HitRecord, Scatter, Ray, Vec3, HitType};
use crate::dot;



#[derive(Debug, Clone, Copy)]
pub struct BoundingBoxStruct {
    pub min_corner: Vec3, 
    pub max_corner: Vec3,
}

pub trait Hit {
    fn hit(&self, ray: &Ray, range: [f64;2]) -> HitType;
    fn bounding_volume(&self) -> Option<BoundingBoxStruct>;
}

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

#[derive(Clone)]
pub struct SphereStruct <'a>{
    pub center: Vec3, 
    pub radius: f64, 
    pub material: &'a dyn Scatter,
}

impl <'a>SphereStruct<'_> {
    pub fn new(center: Vec3, radius: f64, material: &dyn Scatter) -> SphereStruct{
        SphereStruct{center, radius, material}
    }
    fn get_normal(&self, point_on_surface: Vec3) -> Vec3 {
        (point_on_surface - self.center) / self.radius
    }
}
impl<'a> Hit for SphereStruct<'a> {
     fn hit(&self, ray: &Ray, range: [f64;2]) -> HitType {
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
        let hit_point = ray.at(hit_at_t);
        let rec = HitRecord::new(hit_at_t, hit_point, ray.direction, normal)
            .with_material(self.material);
        return HitType::Hit(rec);

    }
    fn bounding_volume(&self) -> Option<BoundingBoxStruct> {
        Some(BoundingBoxStruct::new(
               self.center - self.radius*Vec3::ones(),
               self.center + self.radius*Vec3::ones()
        ))
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
pub enum Rectangle<'a> {
    XY(RectangleStruct<'a>),
    XZ(RectangleStruct<'a>),
    YZ(RectangleStruct<'a>),
}

#[derive(Clone)]
struct RectangleStruct<'a>{
    a_min: f64,
    a_max: f64,
    b_min: f64,
    b_max: f64,
    k: f64,
    material: &'a dyn Scatter
}

impl RectangleStruct<'_> {
    fn new(a_min: f64, a_max: f64, b_min: f64, b_max: f64, k: f64, material: &dyn Scatter) -> RectangleStruct {
        return RectangleStruct{a_min, a_max, b_min, b_max, k, material}
    }
}

impl <'a>Rectangle<'_> {
    pub fn new_xy(x0: f64, x1: f64, y0: f64, y1: f64, k: f64, material: &'a dyn Scatter) -> Rectangle<'a> {
        let x_min = f64::min(x0, x1);
        let x_max = f64::max(x0, x1);
        let y_min = f64::min(y0, y1);
        let y_max = f64::max(y0, y1);
        Rectangle::XY(RectangleStruct::new(x_min,x_max,y_min, y_max, k, material))
    }
    pub fn new_xz(x0: f64, x1: f64, z0: f64, z1: f64, k: f64, material: &'a dyn Scatter) -> Rectangle<'a> {
        let x_min = f64::min(x0, x1);
        let x_max = f64::max(x0, x1);
        let z_min = f64::min(z0, z1);
        let z_max = f64::max(z0, z1);
        Rectangle::XZ(RectangleStruct::new(x_min,x_max,z_min, z_max, k, material))
    }
    pub fn new_yz(y0: f64, y1: f64, z0: f64, z1: f64, k: f64, material: &'a dyn Scatter) -> Rectangle<'a> {
        let y_min = f64::min(y0, y1);
        let y_max = f64::max(y0, y1);
        let z_min = f64::min(z0, z1);
        let z_max = f64::max(z0, z1);
        Rectangle::YZ(RectangleStruct::new(y_min,y_max,z_min, z_max, k, material))
    }
}

impl<'a> Hit for Rectangle<'a> {
    fn hit(&self, ray: &Ray, range: [f64;2]) -> HitType {
        let dim_a;
        let dim_b;
        let dim_c;
        let rect: &RectangleStruct;
        match self {
            Self::XY(r) => {
                dim_a = 0;
                dim_b = 1;
                dim_c = 2;
                rect = r;
            }
            Self::XZ(r) => {
                dim_a = 0;
                dim_b = 2;
                dim_c = 1;
                rect = r;
            }
            Self::YZ(r) => {
                dim_a = 1;
                dim_b = 2;
                dim_c = 0;
                rect = r;
            }
        }
        let t = (rect.k - ray.origin[dim_c]) / ray.direction[dim_c];
        if t < range[0] || t > range[1] {
            return HitType::None
        }
        let a = ray.origin[dim_a] + t * ray.direction[dim_a];
        let b = ray.origin[dim_b] + t * ray.direction[dim_b];
        if a < rect.a_min || a > rect.a_max || b < rect.b_min || b > rect.b_max {
            return HitType::None;
        }
        let mut normal = Vec3::zero();
        normal[dim_c] = 1.0;
        if dot(ray.direction, normal) > 0.0 {
            normal = -normal;
        }
        let hit_point = ray.at(t);
        return HitType::Hit(
            HitRecord::new(t, hit_point, ray.direction, normal)
                .with_material(rect.material)
        );
    }
    fn bounding_volume(&self) -> Option<BoundingBoxStruct> {
        let dim_a;
        let dim_b;
        let dim_c;
        let rect: &RectangleStruct;
        match self {
            Self::XY(r) => {
                dim_a = 0;
                dim_b = 1;
                dim_c = 2;
                rect = r;
            }
            Self::XZ(r) => {
                dim_a = 0;
                dim_b = 2;
                dim_c = 1;
                rect = r;
            }
            Self::YZ(r) => {
                dim_a = 1;
                dim_b = 2;
                dim_c = 0;
                rect = r;
            }
        }
        let mut v_min = Vec3::zero();
        v_min[dim_a] = rect.a_min;
        v_min[dim_b] = rect.b_min;
        v_min[dim_c] = rect.k-0.0001;
        let mut v_max = Vec3::zero();
        v_max[dim_a] = rect.a_max;
        v_max[dim_b] = rect.b_max;
        v_max[dim_c] = rect.k+0.0001;
        return Some(BoundingBoxStruct::new(v_min, v_max));
    }
}

#[derive(Clone)]
pub struct CuboidStruct<'a> {
    sides: [Rectangle<'a>; 6],
    v_max: Vec3,
    v_min: Vec3,
}

impl <'a>CuboidStruct<'_> {
    pub fn new(p0: Vec3, p1: Vec3, material: &'a dyn Scatter) -> CuboidStruct<'a> {

        let mut v_min = Vec3::zero();
        let mut v_max = Vec3::zero();
        for i in 0..3 {
            v_min[i] = f64::min(p0[i], p1[i]);
            v_max[i] = f64::max(p0[i], p1[i]);
        }

        let sides = [
            Rectangle::new_xy(v_min.x(), v_max.x(), v_min.y(), v_max.y(), v_min.z(), material),
            Rectangle::new_xy(v_min.x(), v_max.x(), v_min.y(), v_max.y(), v_max.z(), material),
            Rectangle::new_xz(v_min.x(), v_max.x(), v_min.z(), v_max.z(), v_min.y(), material),
            Rectangle::new_xz(v_min.x(), v_max.x(), v_min.z(), v_max.z(), v_max.y(), material),
            Rectangle::new_yz(v_min.y(), v_max.y(), v_min.z(), v_max.z(), v_min.x(), material),
            Rectangle::new_yz(v_min.y(), v_max.y(), v_min.z(), v_max.z(), v_max.x(), material),
        ];
        return CuboidStruct{sides, v_min, v_max};
    }
}
impl<'a> Hit for CuboidStruct<'a> {
    fn hit(&self, ray: &Ray, range: [f64;2]) -> HitType {
        let mut closest_hit_record = HitType::None;
        for hittable in &self.sides {
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
        return Some(
            BoundingBoxStruct::new(
                self.v_min - 0.0001*Vec3::ones(), 
                self.v_max+0.0001*Vec3::ones()
            ));
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

