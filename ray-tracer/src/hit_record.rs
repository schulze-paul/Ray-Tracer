use crate::{Vec3, Color, Ray, Scatter};
use crate::dot;

#[derive(Copy, Clone, Debug)]
pub struct HitRecord {
    pub t_hit: f64,
    pub hit_point: Vec3,
    pub direction: Vec3,
    pub normal: Vec3,
    pub material: Option<&'static dyn Scatter>,
}

impl <'a>HitRecord {
    pub fn new(t_hit: f64, hit_point: Vec3, direction: Vec3, normal: Vec3) -> HitRecord {
        HitRecord{
            t_hit,
            hit_point,
            direction,
            normal,
            material: None,
        }
    }
    pub fn with_material(mut self, material: &'static dyn Scatter) -> HitRecord {
        self.material = Some(material);
        return self;
    }

    pub fn is_front_face(&self, ray: &Ray) -> bool {
        return dot(ray.direction, self.normal) < 0.0;
    }
    pub fn get_closer(self, other: HitRecord) -> HitRecord {
        if self.t_hit > other.t_hit {
            return other;
        }
        return self;
    }
}

pub struct ScatterRecord<'a> {
    pub hit_record: &'a HitRecord,
    pub probabilities: Vec<f64>,
    pub scattered: Vec<Ray>,
    pub emitted: Color,
}

impl <'a>ScatterRecord<'a> {
    pub fn new(hit_record: &'a HitRecord) -> ScatterRecord<'a> {
        return ScatterRecord {
            hit_record ,
            probabilities: Vec::new(), 
            scattered: Vec::new(),
            emitted: Color::black(),
        };
    }
    pub fn push(mut self, p: f64, direction: Vec3) -> ScatterRecord<'a> {
        self.probabilities.push(p);
        self.scattered.push(
            Ray::new(
                self.hit_record.hit_point,
                direction,
            )
        );
        return self
    }
    pub fn with_emitted(mut self, emitted: Color) -> ScatterRecord<'a> {
        self.emitted = emitted;
        return self;
    }
}
