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
    // println!("{} {:?}", sphere.radius, sphere.center);
    // println!("{:?} {:?}", ray.direction, ray.origin);
    let oc = ray.origin - sphere.center;              // origin to center
    let a = ray.direction.dot(ray.direction); // direction squared
    let b = 2.0 * oc.dot(ray.direction);    // 2 * alignment of center direction and ray direction
    let c = oc.dot(oc) - sphere.radius * sphere.radius; // center distance squared - radius squared
    let discriminant = b * b - 4.0 * a * c;
    

    if discriminant > 0.0 {
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
        return true;
    }
    return false;
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
