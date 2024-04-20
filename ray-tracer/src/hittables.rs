use crate::{HitRecord, Material, Ray, Vec3};
use crate::dot;



#[derive(Debug, Clone)]
pub enum Hittable <'a>{
    HittableList(HittableListStruct<'a>),
    Sphere(SphereStruct<'a>)
}

#[derive(Debug, Clone, Copy)]
pub struct SphereStruct <'a>{
    pub center: Vec3, 
    pub radius: f64, 
    pub material: &'a Material}

#[derive(Debug, Clone)]
pub struct HittableListStruct <'a>{
    list: Vec<&'a Hittable<'a>>
}

pub trait Hit<'a> {
    fn hit(&'a self, ray: &'a Ray, range: [f64;2]) -> Option<HitRecord>;
}

pub trait MaterialTrait {
    fn material(&self) -> Option<&Material>;
}


impl <'a>Hit<'a> for Hittable<'a> {
    fn hit(&'a self, ray: &'a Ray, range: [f64;2]) -> Option<HitRecord> {
        match self {
            Hittable::Sphere(s) => s.hit(ray, range),
            Hittable::HittableList(l) => l.hit(ray, range),
        }
    }
}

impl <'a>MaterialTrait for Hittable<'_> {
    fn material(&self) -> Option<&Material> {
        match self {
            Hittable::Sphere(s) => Some(&s.material),
            Hittable::HittableList(_) => None
        }
    }
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
    fn hit(&'a self, ray: &'a Ray, range: [f64;2]) -> Option<HitRecord> {
        let mut closest_hit_record = None;
        for hittable in &self.list {
            let hit_record = hittable.hit(ray, range);
            match hit_record {
                None => continue,
                Some(h) => {
                    match closest_hit_record {
                        None => closest_hit_record = Some(h),
                        Some(ref c) => {
                            if h.t_hit < c.t_hit {
                                closest_hit_record = Some(h) 
                            }
                        }
                    }
                }
            }
        }
        return closest_hit_record;
    }
}

impl <'a>SphereStruct<'_> {
    pub fn new(center: Vec3, radius: f64, material: &Material) -> SphereStruct{
        SphereStruct{center, radius, material}
    }
    pub fn hit(&'a self, ray: &'a Ray, range: [f64;2]) -> Option<HitRecord> {
        let oc = ray.origin - self.center;              // origin to center
        let a = dot(ray.direction, ray.direction);      // direction squared
        let b = 2.0 * dot(oc, ray.direction);           // 2 * alignment of center direction and ray direction
        let c = dot(oc,oc) - self.radius * self.radius; // center distance squared - radius squared
        let discriminant = b * b - 4.0 * a * c;

        if discriminant < 0.0 {
            return None; // no hit
        }
        // ray in direction of sphere
        let mut hit_at_t = (-b - discriminant.sqrt()) / (2.0 * a);
        if !(hit_at_t < range[1] && hit_at_t > range[0]) {
            // not in range, try other hit
            hit_at_t = (-b + discriminant.sqrt()) / (2.0 * a);
            if !(hit_at_t < range[1] && hit_at_t > range[0])
            {
                // not in range, no hit
                return None;
            }
        }
        let normal = self.get_normal(ray.at(hit_at_t));
        let rec = HitRecord::new(hit_at_t ,ray, normal)
            .with_material(self.material);
        return Some(rec);

    }
    fn get_normal(&self, point_on_surface: Vec3) -> Vec3 {
        (point_on_surface - self.center) / self.radius
    }
}
