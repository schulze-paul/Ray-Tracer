mod ray;
mod vec3;
mod hit_record;
mod hittables;
mod camera;

use std::f64;

use hit_record::HitRecord;
use ray::Ray;
use vec3::{Vec3, dot, cross};
use Vec3 as Color;
use crate::camera::{Camera, ImageData};
use hittables::{Hittable, SphereStruct, Hit, hit_sphere};

fn main() {
    let width = 200;
    let height = 200;
    let image_data = ImageData::new(width, height);

    let mut camera = Camera::new(image_data);
    camera.look_from(Vec3::new(0.0, 0.0, 0.0));
    camera.look_at(Vec3::new(0.0, 0.0, 8.0));
    camera.set_vfov(45.0);
    let sphere_struct = SphereStruct::new(
            Vec3::new(0.0,0.0,8.0), 1.0
        );
    camera.look_at(sphere_struct.center);
    

    for index_u in 0..camera.image_data.width {
        for index_v in 0..camera.image_data.height {
            let u: f64 = index_u as f64 / (camera.image_data.width  - 1) as f64;
            let v: f64 = index_v as f64 / (camera.image_data.height - 1) as f64;
            
            let ray = camera.get_ray(u, v);
            let mut hit_record = HitRecord::new();

            if hit_sphere(&sphere_struct, &ray, [-1e20, 1e20], &mut hit_record) {
            // if f64::sqrt(ray.direction.y().powi(2) + ray.direction.z().powi(2)) < 2.0 {
                camera.image_data.set(index_u, index_v, Color::new(1.0, 1.0, 1.0));
            }
        }
    }
    match camera.image_data.write(String::from("first_test.ppm")) {
        Ok(_) => {println!("Ok");},
        Err(e) => {println!("{}", e);},
    };
}
