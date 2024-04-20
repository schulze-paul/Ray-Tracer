use crate::{Vec3, Color, Ray, HitRecord};
use crate::{dot, random_float};

#[derive(Debug)]
pub enum Material {
    Metal(MetalStruct),
    Dielectric(DielectricStruct),
    Lambertian(LambertianStruct)
}



pub trait Scatter {
    fn scatter(&self, ray: &Ray, hit_record: &HitRecord) -> Ray;
}
impl Scatter for Material {
    fn scatter(&self, ray: &Ray, hit_record: &HitRecord) -> Ray {
        match self {
            Material::Metal(m) =>      m.scatter(ray, hit_record),
            Material::Dielectric(d) => d.scatter(ray, hit_record),
            Material::Lambertian(l) => l.scatter(ray, hit_record),
        }
    }
}

pub trait Attenuation {
    fn attenuation(&self) -> Color;
}
impl Attenuation for Material {
    fn attenuation(&self) -> Color {
        match self {
            Material::Metal(m) =>      m.attenuation(),
            Material::Dielectric(d) => d.attenuation(),
            Material::Lambertian(l) => l.attenuation(),
        }
    }        
}

#[derive(Debug)]
pub struct MetalStruct {
    albedo: Color,
    fuzz: f64
}
impl MetalStruct {
    pub fn new(albedo: Color, fuzz: f64) -> MetalStruct {
        MetalStruct{albedo, fuzz}
    }
    fn scatter(&self, ray: &Ray, hit_record: &HitRecord) -> Ray {
        return Ray::new(
            // move away from hit point to avoid double hit
            hit_record.hit_point, //+ hit_record.normal/1024.0, 
            reflect(ray.direction, hit_record.normal))
    }
    fn attenuation(&self) -> Color {
        return self.albedo;
    }
}

fn reflect(v: Vec3, n: Vec3) -> Vec3 {
    return v - 2.0 * dot(v, n) * n;
}

#[derive(Debug)]
pub struct DielectricStruct {
    refractive_index: f64
}
impl DielectricStruct {
    pub fn new(refractive_index: f64) -> DielectricStruct {
        DielectricStruct{refractive_index}
    }
    fn scatter(&self, ray: &Ray, hit_record: &HitRecord) -> Ray {
        let mut refraction_ratio = self.refractive_index;
        let mut unit_normal = -hit_record.normal;
        if hit_record.is_front_face(ray) {
            refraction_ratio = 1.0/refraction_ratio;
            unit_normal = -unit_normal;
        }
        let cos_theta = f64::min(dot(-ray.direction, unit_normal), 1.0);
        let sin_theta = (1.0 - cos_theta * cos_theta).sqrt();

        let cannot_refract = refraction_ratio * sin_theta > 1.0;
        let reflected = cannot_refract || 
                        reflectance(cos_theta, refraction_ratio) > random_float(0.0, 1.0);

        let scattered = 
            if reflected {reflect(ray.direction, unit_normal) }
            else {refract(ray.direction, unit_normal, refraction_ratio)};

        return Ray::new(
            hit_record.hit_point,
            scattered)

    }
    fn attenuation(&self) -> Color {
        return Color::white();
    }
}
fn refract(v_in: Vec3, normal: Vec3, refraction_ratio: f64) -> Vec3 {
    let cos_theta = f64::min(dot(-v_in, normal), 1.0);
    let out_perpendicular = refraction_ratio * (v_in + cos_theta*normal);
    let out_parallel = -(1.0 - out_perpendicular.length_squared()).abs().sqrt() * normal;
    return out_perpendicular + out_parallel;

}
fn reflectance(cosine: f64, ref_idx: f64) -> f64 {
    // Use Schlick's approximation for reflectance.
    let r0 = (
            (1.0 - ref_idx) / (1.0 + ref_idx)
        ).powi(2);
    return r0 + (1.0 - r0) * (1.0 - cosine).powi(5);
}



#[derive(Debug)]
pub struct LambertianStruct {
    color: Color
}

impl LambertianStruct {
    pub fn new(color: Color) -> LambertianStruct {
        LambertianStruct{color}
    }
    fn scatter(&self, _ray: &Ray, hit_record: &HitRecord) -> Ray {
        Ray::new(
            hit_record.hit_point,
            hit_record.normal + Vec3::random_in_unit_sphere().unit(),
        ) 
    }
    fn attenuation(&self) -> Color {
        return self.color;
    }
}


