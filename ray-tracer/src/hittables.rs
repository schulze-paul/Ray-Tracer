use std::fmt::write;

use crate::Vec3;
use crate::ray::Ray;
use crate::hit_record::HitRecord;

enum Hittable {
    Sphere(SphereStruct)
}

struct SphereStruct{pub center: Vec3, pub radius: f64}


trait Hit {
    fn hit(&self, ray: &Ray, range: [f64;2], rec: &mut HitRecord) -> bool;
}


impl SphereStruct{
    pub fn new(center: Vec3, radius: f64) {
        SphereStruct{center, radius};
    }
}

impl Hit for SphereStruct {
    fn hit(&self, ray: &Ray, range: [f64;2], rec: &mut HitRecord) -> bool {
        let oc: Vec3 = ray.origin - self.center;
        
        let a = ray.direction.dot(ray.direction);
        let b = 2.0 * oc.dot(ray.direction);
        let c = oc.dot(oc) - self.radius*self.radius;
        let discriminant = b*b - 4.0*a*c;

        if discriminant < 0.0 {
            return false; // NO HIT
        }
        
        // Check if hit point in range
        let t_hit = [
            -(b + discriminant.sqrt()) / (2.0 * a),
            -(b - discriminant.sqrt()) / (2.0 * a)
        ];
        for t in t_hit.into_iter().filter(|t| {t > &range[0] && t < &range[1]}) {
            self.set_hit_record(t, ray, rec);
            return true;
        }
        return false;
    }
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
