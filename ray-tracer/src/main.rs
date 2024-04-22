mod ray;
mod vec3;
mod color;
mod hit_record;
mod hittables;
mod material;
mod camera;
mod background;

use std::f64;
use rand::Rng;

use hit_record::{HitType, HitRecord};
use ray::Ray;
use vec3::{Vec3, dot, cross};
use color::Color;
use camera::{Camera, ImageData};
use hittables::{BVHNodeStruct, Hit, Hittable, HittableListStruct, MaterialTrait, SphereStruct};
use background::GradientBackground;
use material::{Attenuation, DielectricStruct, LambertianStruct, Material, MetalStruct, Scatter};

fn main() {
    // image and camera
    let width = 500;
    let height = 500;
    let num_samples = 1;
    let image_data = ImageData::new(width, height, num_samples);

    // materials
    let metal = Material::Metal(MetalStruct::new(Color::white(), 0.0));
    let dielectric = Material::Dielectric(DielectricStruct::new(1.4));
    let red_lambertian = Material::Lambertian(LambertianStruct::new(Color::red()));
    let grey_lambertian = Material::Lambertian(LambertianStruct::new(Color::grey(0.3)));

    // objects
    let small_r = 2.0;
    let big_r = 1000.0;
    let sphere_center = small_r*Vec3::y_hat();
    let sphere_metal = Hittable::Sphere(SphereStruct::new(
            sphere_center,
            small_r,
            &metal
        ));
    let sphere_red = Hittable::Sphere(SphereStruct::new(
            sphere_center + 2.5*small_r*Vec3::x_hat(),
            small_r,
            &red_lambertian
        ));
    let sphere_glass = Hittable::Sphere(SphereStruct::new(
            sphere_center - 2.5*small_r*Vec3::x_hat(),
            small_r,
            &dielectric
        ));
    let ground = Hittable::Sphere(SphereStruct::new(
            -big_r*Vec3::y_hat(), 
            big_r, 
            &grey_lambertian
        ));
    let mut world = HittableListStruct::new()
        .push(&sphere_metal)
        .push(&sphere_red)
        .push(&sphere_glass)
        .push(&ground);
    


    let mut camera = Camera::new(image_data)
        .look_from(Vec3::new(0.0, 4.0, -12.0))
        .set_vfov(60.0)
        .look_at(sphere_center)
        .focus_on_look_at()
        .set_aperture(0.0);
    let background = GradientBackground::new(
        Color::new(0.1, 0.5, 0.7), Color::white()
    );
    let world_size = world.list.len();
    let bvh = Hittable::BHVNode(BVHNodeStruct::new(&mut world, 0, world_size));

    for i_samples in 0..num_samples {
        for index_u in 0..camera.image_data.width {
            for index_v in 0..camera.image_data.height {
                let u: f64 = index_u as f64 / (camera.image_data.width  - 1) as f64;
                let v: f64 = index_v as f64 / (camera.image_data.height - 1) as f64;
                
                let ray_in = camera.get_ray(u, v);

                camera.image_data.add(index_u, index_v, 
                    cast_ray(ray_in, &bvh, &background, 0)
                );
            }
        }
    }
    match camera.image_data.write(String::from("first_test.ppm")) {
        Ok(_) => {println!("Ok");},
        Err(e) => {println!("{}", e);},
    };
}

fn cast_ray(ray_in: Ray, world: &Hittable, background: &GradientBackground, depth: i32) -> Color {
    let max_depth = 16;
    if depth >= max_depth {
        return Color::black();
    }
    let hit_record = world.hit(&ray_in, [0.1,1e20]);

    return match hit_record {
        HitType::Hit(h) => {
            match h.material.zip(h.scattered) {
                None => Color::black(),
                Some((m, s)) => {
                    m.attenuation() * cast_ray(s, world, background, depth+1)
                }
            }
        }
        _ => background.get_color(ray_in),
    }
}

fn random_float(min: f64, max: f64) -> f64 {
    let mut rng = rand::thread_rng();
    return rng.gen_range(min..max);
}
