use crate::{Vec3, Color, Ray, Material, Scatter};
use crate::dot;


#[derive(Debug, Clone)]
pub enum HitType <'a>{
    Hit(HitRecord<'a>),
    BoundingHit,
    None,
}

#[derive(Debug, Clone)]
pub struct HitRecord <'a> {
    pub t_hit: f64,
    pub ray: &'a Ray,
    pub hit_point: Vec3,
    pub normal: Vec3,
    pub material: Option<&'a Material>,
}

impl <'a>HitRecord <'a>{
    pub fn new(t_hit: f64, ray: &'a Ray, normal: Vec3) -> HitRecord<'a> {
        let hit_point = ray.at(t_hit);
        HitRecord{
            t_hit,
            ray,
            hit_point,
            normal,
            material: None,
        }
    }
    pub fn with_material(mut self, material: &'a Material) -> HitRecord<'a> {
        self.material = Some(material);
        return self;
    }

    pub fn is_front_face(&self, ray: &Ray) -> bool {
        return dot(ray.direction, self.normal) < 0.0;
    }
}


pub struct ScatterRecord<'a> {
    pub hit_record: &'a HitRecord<'a>,
    pub probabilities: Vec<f64>,
    pub scattered: Vec<Ray>,
}

impl <'a>ScatterRecord<'a> {
    pub fn new(hit_record: &'a HitRecord<'a>) -> ScatterRecord<'a> {
        return ScatterRecord{
           hit_record ,
           probabilities: Vec::new(), 
           scattered: Vec::new()
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
}
