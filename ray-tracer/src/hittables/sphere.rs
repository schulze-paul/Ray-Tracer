use std::f64::consts::PI;
use std::rc::Rc;

use crate::{random_float, BoundingBox, Hit, HitRecord, Ray, Scatter, Vec3, ONB, Interval};
use crate::dot;

// #[derive(Clone)]
pub struct Sphere{
    pub center: Vec3, 
    pub radius: f64, 
    pub material: Rc<dyn Scatter>,
}

impl <'a>Sphere<>{
    pub fn new(center: Vec3, radius: f64, material: Rc<dyn Scatter>) -> Sphere{
        Sphere{center, radius, material}
    }
    fn get_normal(&self, point_on_surface: Vec3) -> Vec3 {
        (point_on_surface - self.center) / self.radius
    }
    fn random_to_sphere(&self, distance_squared: f64) -> Vec3 {
        let r1 = random_float(0.0, 1.0);
        let r2 = random_float(0.0, 1.0);
        let z = 1.0 + r2*((1.0-self.radius*self.radius/distance_squared).sqrt() - 1.0);

        let phi = 2.0*PI*r1;
        let x =phi.cos()*(1.0-z*z).sqrt();
        let y =phi.sin()*(1.0-z*z).sqrt();

        return Vec3::new(x, y, z);

    }
}
impl<'a> Hit for Sphere {
     fn hit(&self, ray: &Ray, range: Interval) -> Option<HitRecord> {
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
        if !range.contains(hit_at_t) {
            // not in range, try other hit
            hit_at_t = (-b + discriminant.sqrt()) / (2.0 * a);
            if !range.contains(hit_at_t) {
                // not in range, no hit
                return None;
            }
        }
        let normal = self.get_normal(ray.at(hit_at_t));
        let hit_point = ray.at(hit_at_t);
        let rec = HitRecord::new(hit_at_t, hit_point, ray.direction, normal, self.material.clone());
        return Some(rec);

    }
    fn bounding_volume(&self) -> BoundingBox {
        BoundingBox::new(
               self.center - self.radius*Vec3::ones(),
               self.center + self.radius*Vec3::ones()
        )
    }
    fn random_to_surface(&self, origin: Vec3) -> Option<Vec3> {
        let direction = self.center - origin;
        let distance_squared = direction.length_squared();
        let uvw = ONB::from_w(direction);
        return Some(uvw.local_from_vec(self.random_to_sphere(distance_squared)));
    }
    fn pdf_value(&self, origin: Vec3, direction: Vec3) -> f64 {
        match self.hit(&Ray::new(origin, direction), Interval::new(0.01, 1e20)) {
            Some(_) => {
                let cos_theta_max = (1.0 - self.radius.powi(2)/(self.center - origin).length_squared()).sqrt();
                let solid_angle = 2.0*PI*(1.0 - cos_theta_max);
                return 1.0/solid_angle;
            }
            _ => return 0.0,
        }
    }
}
