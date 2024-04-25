use rand::Rng;

mod ray;
mod vec3;
mod color;
mod hit_record;
mod hittables;
mod material;
mod camera;
mod background;
mod shaders;
mod image_data;
mod onb;
mod pdf;

use hit_record::{HitRecord, ScatterRecord};
use ray::Ray;
use vec3::{Vec3, dot, cross};
use color::Color;
use camera::Camera;
use image_data::ImageData;
use hittables::{Hit, Interval};
use hittables::hittable_list::HittableList;
use hittables::sphere::Sphere;
use hittables::cuboid::Cuboid;
use hittables::bounding_volume::{BVHNode, BoundingBox};
use background::GradientBackground;
use material::{Scatter, dielectric::Dielectric, lambertian::Lambertian, metal::Metal, emissive::Emissive};
use shaders::{Shader, RayTracer, NormalShader, DepthShader, ScatterShader};
use onb::ONB;
use pdf::PDF;

fn main() {
    // image and camera
    let height = 500;
    let width = 500;
    let num_samples: usize = 128;
    let image_data = ImageData::new(width as usize, height as usize, num_samples);

    // materials
    let metal = Metal::new(Color::white(), 0.0);
    let dielectric = Dielectric::new(1.4);
    let red_lambertian = Lambertian::new(Color::red());
    let white_lambertian = Lambertian::new(Color::white());
    let emissive = Emissive::new(10.0*Color::white());

    // objects
    let small_r = 2.0;
    let box_spacing = 3.0*small_r;
    let sphere_center = 0.5*box_spacing*Vec3::new(1.0, -2.0, -1.0) + small_r*Vec3::y_hat();
    let sphere_metal = Sphere::new(
            sphere_center,
            small_r,
            &metal
        );
    let sphere_red = Sphere::new(
            sphere_center + box_spacing*Vec3::new(0.0, 1.0, 1.0),
            small_r,
            &red_lambertian
        );
    let sphere_glass = Sphere::new(
            sphere_center + box_spacing*Vec3::new(-1.0, 1.0, 0.0),
            small_r,
            &dielectric
        );
    let sphere_emissive = Sphere::new(
            sphere_center + box_spacing*Vec3::new(0.0, 2.0, 2.0),
            small_r,
            &emissive
        );
    
    let box1 = Cuboid::new(
        Vec3::new( 0.0*box_spacing, -1.0*box_spacing,  0.0*box_spacing),
        Vec3::new(-1.0*box_spacing,  0.0*box_spacing, -1.0*box_spacing),
        &white_lambertian
    );
    let box2 = Cuboid::new(
        Vec3::new( 0.0*box_spacing, -1.0*box_spacing,  0.0*box_spacing),
        Vec3::new( 1.0*box_spacing,  0.0*box_spacing,  1.0*box_spacing),
        &white_lambertian
    );
    let box3 = Cuboid::new(
        Vec3::new( 0.0*box_spacing, -1.0*box_spacing,  0.0*box_spacing),
        Vec3::new( 1.0*box_spacing, -2.0*box_spacing, -1.0*box_spacing),
        &white_lambertian
    );
    
    let mut world = HittableList::new()
        .push(&sphere_metal)
        .push(&sphere_red)
        .push(&sphere_glass)
        .push(&sphere_emissive)
        .push(&box1)
        .push(&box2)
        .push(&box3)
    ;


    let mut camera = Camera::new(image_data)
        .look_from(Vec3::new(12.0, 8.0, -12.0))
        .with_vfov(90.0)
        .look_at(sphere_center)
        .focus_on_look_at()
        .with_aperture(0.0)
        .with_num_samples(num_samples);
    let background = GradientBackground::new(
        Color::black(),
        Color::black()
    //    Color::new(0.1, 0.5, 0.7), 
    //    Color::white()
    );
    let world_size = world.list.len();
    let bvh = BVHNode::new(&mut world, 0, world_size);

    camera.render(&bvh, background);


    camera.image.write(String::from("rendered/ray_tracer.ppm")).unwrap();
    camera.normal_image.write(String::from("rendered/normals.ppm")).unwrap();
    camera.scatter_image.write(String::from("rendered/scatter.ppm")).unwrap();
    camera.depth_image.write(String::from("rendered/depth.ppm")).unwrap();
}

fn random_float(min: f64, max: f64) -> f64 {
    let mut rng = rand::thread_rng();
    return rng.gen_range(min..max);
}

fn random_usize(min: usize, max: usize) -> usize {
    let mut rng = rand::thread_rng();
    return rng.gen_range(min..max);
}

