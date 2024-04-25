use crate::vec3::cross;
use crate::{random_float, random_usize, BoundingBox, Hit, HitRecord, Interval, Ray, Scatter, Vec3};
use crate::dot;

#[derive(Clone)]
pub struct Quad<'a> {
    corner: Vec3,
    u: Vec3,
    v: Vec3,
    w: Vec3,
    normal: Vec3,
    d: f64,
    area: f64,
    material: &'a dyn Scatter,
}

impl<'a> Quad<'_> {
    pub fn new(corner: Vec3, u: Vec3, v: Vec3, material: &dyn Scatter) -> Quad {
        let n = cross(u, v);
        let normal = n.unit();
        let d = dot(normal, corner);
        let w = n/dot(n,n);
        let area = n.length();
        return Quad {corner, u, v, w, normal, d, area, material};
    }
    fn contains(&self, alpha: f64, beta: f64) -> bool {
        let inside = Interval::new(0.0, 1.0);
        return inside.contains(beta) && !inside.contains(alpha);
    }
}

impl Hit for Quad<'_> {
    fn hit(&self, ray: &Ray, range: Interval) -> Option<HitRecord> {
        let denom = dot(self.normal, ray.direction);
        if denom.abs() < 1e-8 {
            return None // parallel to plane
        }
        let t = (self.d - dot(self.normal, ray.origin))/denom;

        if !range.contains(t) {
            return None;
        }
        let intersection = ray.at(t);
        let local_hitpoint = intersection - self.corner;
        let alpha = dot(self.w, cross(local_hitpoint, self.v));
        let beta = dot(self.w, cross(self.u, local_hitpoint));

        if !self.contains(alpha, beta) {
            return None;
        }
        return Some(HitRecord::new(t, intersection, ray.direction, self.normal));
    } 
    fn bounding_volume(&self) -> Option<BoundingBox> {
        return Some(BoundingBox::surrounding(
            BoundingBox::new(self.corner, self.corner + self.u + self.v),
            BoundingBox::new(self.corner + self.u, self.corner + self.v),
        ));
    }
    fn pdf_value(&self, origin: Vec3, direction: Vec3) -> f64 {
        match self.hit(&Ray::new(origin, direction), Interval::new(0.001, f64::INFINITY)) {
            Some(h) => {
                let distance_squared = h.t_hit * h.t_hit * direction.length_squared();
                let cosine = dot(direction, h.normal) / direction.length();
                return distance_squared / (cosine * self.area);
            },
            _ => return 0.0,
        }
    }
    fn random_to_surface(&self, origin: Vec3) -> Option<Vec3> {
        let point = self.corner 
            + random_float(0.0, 1.0) * self.u
            + random_float(0.0, 1.0) * self.w;
        return Some(point - origin);
    }
}


#[derive(Clone)]
pub struct Cuboid<'a> {
    sides: [Quad<'a>; 6],
    p0: Vec3,
    p1: Vec3,
}

impl <'a>Cuboid<'_> {
    pub fn new(p0: Vec3, p1: Vec3, material: &'a dyn Scatter) -> Cuboid<'a> {
        
        let x_diff = (p1 - p0).x() * Vec3::x_hat();
        let y_diff = (p1 - p0).y() * Vec3::y_hat();
        let z_diff = (p1 - p0).z() * Vec3::z_hat();

        let sides = [
            Quad::new(p0, x_diff, y_diff, material),
            Quad::new(p0, x_diff, z_diff, material),
            Quad::new(p0, y_diff, z_diff, material),
            Quad::new(p1,-x_diff,-y_diff, material),
            Quad::new(p1,-x_diff,-z_diff, material),
            Quad::new(p1,-y_diff,-z_diff, material),
        ];
        return Cuboid{sides, p0, p1};
    }
}
impl<'a> Hit for Cuboid<'a> {
    fn hit(&self, ray: &Ray, range: Interval) -> Option<HitRecord> {
        let mut closest_hit_record = None;
        for hittable in &self.sides {
            let hit_record = hittable.hit(ray, range);
            match (closest_hit_record, hit_record) {
                (None, Some(_)) => 
                    closest_hit_record = hit_record,
                (Some(ch), Some(h)) => 
                    closest_hit_record = Some(ch.get_closer(h)),
                _ => continue,
            }
        }
        return closest_hit_record;
    }
    fn bounding_volume(&self) -> Option<BoundingBox> {
        return Some(
            BoundingBox::new(
                self.p0-0.0001*Vec3::ones(), 
                self.p1+0.0001*Vec3::ones()
            ));
    }
    fn pdf_value(&self, origin: crate::vec3::Vec3, direction: crate::vec3::Vec3) -> f64 {
        let weight = 1.0/(self.sides.len() as f64);
        return self.sides.iter().map(|ref s| weight * s.pdf_value(origin, direction)).sum();
    }
    fn random_to_surface(&self, origin: crate::vec3::Vec3) -> Option<Vec3> {
        let random_index = random_usize(0, self.sides.len());
        return self.sides[random_index].random_to_surface(origin);
    }
}
