use crate::{Vec3, Ray, Hit, Scatter, HitType, HitRecord, BoundingBoxStruct};
use crate::dot;

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
