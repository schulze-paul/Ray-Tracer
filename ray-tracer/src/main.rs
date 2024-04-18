mod ray;
mod vec3;
mod hit_record;
mod hittables;
mod camera;
mod background;

use std::f64;

use hit_record::HitRecord;
use ray::Ray;
use vec3::{Vec3, dot, cross};
use Vec3 as Color;
use camera::{Camera, ImageData};
use hittables::{Hittable, SphereStruct, Hit, };
use background::GradientBackground;

fn main() {
    let width = 200;
    let height = 200;
    let image_data = ImageData::new(width, height);

    let mut camera = Camera::new(image_data);
    camera.look_from(Vec3::zero());
    camera.set_vfov(45.0);
    let sphere = SphereStruct::new(
            Vec3::new(0.0,0.0,8.0), 
            2.0
        );
    camera.look_at(sphere.center);
    let hittable = Hittable::Sphere(sphere);
    let background = GradientBackground::new(
        Color::zero(),
        Color::one()
    );

    for index_u in 0..camera.image_data.width {
        for index_v in 0..camera.image_data.height {
            let u: f64 = index_u as f64 / (camera.image_data.width  - 1) as f64;
            let v: f64 = index_v as f64 / (camera.image_data.height - 1) as f64;
            
            let ray_in = camera.get_ray(u, v);
            let mut hit_record = HitRecord::new();

            camera.image_data.set(index_u, index_v, 
                get_color(ray_in, &hittable, &background, &mut hit_record)
            );
        }
    }
    match camera.image_data.write(String::from("first_test.ppm")) {
        Ok(_) => {println!("Ok");},
        Err(e) => {println!("{}", e);},
    };
}

fn get_color(ray_in: Ray, world: &Hittable, background: &GradientBackground, hit_record: &mut HitRecord) -> Color {
    if !world.hit(&ray_in, [-1e20,1e20], hit_record) {
        // return Color::zero();
        return background.get_color(ray_in);
    }
    return Color::new(0.0, 0.0, 1.0)
}
