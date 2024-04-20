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
    pub scattered: Option<Ray>,
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
            scattered: None,
        }
    }
    pub fn with_material(mut self, material: &'a Material) -> HitRecord<'a> {
        self.material = Some(material);
        self.scattered = Some(material.scatter(self.ray, &self));
        return self;
    }
    pub fn with_scattered(mut self, scattered: &'a Ray) -> HitRecord<'a> {
        self.scattered = Some(*scattered);
        return self
    }


    pub fn is_front_face(&self, ray: &Ray) -> bool {
        return dot(ray.direction, self.normal) < 0.0;
    }
}
