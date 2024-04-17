use crate::Vec3;
use crate::ray::Ray;
use crate::hit_record::HitRecord;

pub enum Hittable {
    Sphere(SphereStruct)
}

pub struct SphereStruct{pub center: Vec3, pub radius: f64}


pub trait Hit {
    fn hit(&self, ray: &Ray, range: [f64;2], rec: &mut HitRecord) -> bool;
}

impl SphereStruct{
    pub fn new(center: Vec3, radius: f64) -> SphereStruct{
        SphereStruct{center, radius}
    }
}

impl Hit for Hittable {
    fn hit(&self, ray: &Ray, range: [f64;2], rec: &mut HitRecord) -> bool {
        match self {
            Hittable::Sphere(s) => hit_sphere(s, ray, range, rec),
        }
    }
}

fn hit_sphere(sphere: &SphereStruct, ray: &Ray, range: [f64;2], rec: &mut HitRecord) -> bool {
    let oc: Vec3 = ray.origin - sphere.center;
    
    let a = ray.direction.dot(ray.direction);
    let b = 2.0 * oc.dot(ray.direction);
    let c = oc.dot(oc) - sphere.radius*sphere.radius;
    let discriminant = b*b - 4.0*a*c;

    if discriminant < 0.0 {
        return false; // NO HIT
    }
    /* 
    // Check if hit point in range
    let t_hit = [
        -(b + discriminant.sqrt()) / (2.0 * a),
        -(b - discriminant.sqrt()) / (2.0 * a)
    ];
    for t in t_hit.into_iter().filter(|t| {t > &range[0] && t < &range[1]}) {
        sphere.set_hit_record(t, ray, rec);
        return true;
    }
    return false;
    */ 
    let mut t_hit = (-b - discriminant.sqrt()) / 2.0 / a;
    if !(t_hit > range[0] && t_hit < range[1]) {
        t_hit = (-b + discriminant.sqrt()) / 2.0 / a;
        if !(t_hit > range[0] && t_hit < range[1]) {
            // not in range
            return false;
        }
    }
    return true;
}

impl SphereStruct {
    fn get_normal(&self, point_on_surface: Vec3) -> Vec3 {
        (point_on_surface - self.center) / self.radius
    }
    fn set_hit_record(&self, t_hit: f64, ray: &Ray, rec: &mut HitRecord) {
        rec.is_hit = true;
        rec.t_hit = t_hit;
        rec.hit_point = ray.at(t_hit);
        rec.normal = self.get_normal(rec.hit_point);
    }
}
