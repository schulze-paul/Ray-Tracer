use crate::Vec3;
use crate::ray::Ray;
use crate::hit_record::HitRecord;
use crate::dot;

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
            Hittable::Sphere(s) => s.hit(ray, range, rec),
        }
    }
}


impl SphereStruct {
    pub fn hit(&self, ray: &Ray, range: [f64;2], rec: &mut HitRecord) -> bool {
        let oc = ray.origin - self.center;              // origin to center
        let a = dot(ray.direction, ray.direction); // direction squared
        let b = 2.0 * dot(oc, ray.direction);    // 2 * alignment of center direction and ray direction
        let c = dot(oc,oc) - self.radius * self.radius; // center distance squared - radius squared
        let discriminant = b * b - 4.0 * a * c;

        if discriminant < 0.0 {
            return false;
        }
        // ray in direction of sphere
        let mut hit_at_t = (-b - discriminant.sqrt()) / (2.0 * a);
        if !(hit_at_t < range[1] && hit_at_t > range[0]) {
            // not in range, try other hit
            hit_at_t = (-b + discriminant.sqrt()) / (2.0 * a);
            if !(hit_at_t < range[1] && hit_at_t > range[0])
            {
                // not in range, no hit
                return false;
            }
        }
        rec.is_hit = true;
        rec.t_hit = hit_at_t;
        rec.hit_point = ray.at(rec.t_hit);
        rec.normal = self.get_normal(rec.hit_point);
        return true;
    }
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
