mod ray;
mod vec3;
mod hit_record;
mod hittables;
mod material;
mod camera;
mod background;

use std::f64;

use hit_record::HitRecord;
use ray::Ray;
use vec3::{Vec3, dot, cross};
use Vec3 as Color;
use camera::{Camera, ImageData};
use hittables::{Hittable, SphereStruct, Hit, MaterialTrait};
use background::GradientBackground;
use material::{DielectricStruct, Material, MetalStruct, Scatter};

fn main() {
    // image and camera
    let width = 500;
    let height = 500;
    let num_samples = 10;
    let image_data = ImageData::new(width, height, num_samples);
    let mut camera = Camera::new(image_data);
    camera.look_from(Vec3::zero());
    camera.set_vfov(45.0);

    // materials
    let metal = Material::Metal(MetalStruct::new(Color::one(), 0.0));
    let dielectric = Material::Dielectric(DielectricStruct::new(1.4));

    // objects
    let sphere = SphereStruct::new(
            Vec3::new(0.0,0.0,8.0), 
            2.0,
            dielectric
        );
    camera.look_at(sphere.center);
    let hittable = Hittable::Sphere(sphere);
    let background = GradientBackground::new(
        Color::new(0.1, 0.5, 0.7), Color::new(1.0, 1.0, 1.0)
    );

    for i_samples in 0..num_samples {
        for index_u in 0..camera.image_data.width {
            for index_v in 0..camera.image_data.height {
                let u: f64 = index_u as f64 / (camera.image_data.width  - 1) as f64;
                let v: f64 = index_v as f64 / (camera.image_data.height - 1) as f64;
                
                let ray_in = camera.get_ray(u, v);

                camera.image_data.add(index_u, index_v, 
                    get_color(ray_in, &hittable, &background, 0)
                );
            }
        }
    }
    match camera.image_data.write(String::from("first_test.ppm")) {
        Ok(_) => {println!("Ok");},
        Err(e) => {println!("{}", e);},
    };
}

fn get_color(ray_in: Ray, world: &Hittable, background: &GradientBackground, depth: i32) -> Color {
    let max_depth = 16;
    // if depth >= max_depth {
    //     return Color::zero();
    // }


    let mut hit_record = HitRecord::new();
    if !world.hit(&ray_in, [0.0,1e20], &mut hit_record) {
        return background.get_color(ray_in);
    }
    match world.material() {
        None => {
            return Color::zero();
        }
        Some(m) => {
            let scattered = m.scatter(&ray_in, &hit_record);
            return get_color(scattered, world, background, depth+1);
        }
    }
}

