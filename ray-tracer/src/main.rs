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

use rand::Rng;

use hit_record::{HitType, HitRecord, ScatterRecord};
use ray::Ray;
use vec3::{Vec3, dot, cross};
use color::Color;
use camera::Camera;
use image_data::ImageData;

use hittables::Hit;
use hittables::hittable_list::HittableListStruct;
use hittables::sphere::SphereStruct;
use hittables::cuboid::CuboidStruct;
use hittables::bounding_volume::{BVHNodeStruct, BoundingBoxStruct};

use background::GradientBackground;
use material::{dielectric::DielectricStruct, lambertian::LambertianStruct, metal::MetalStruct, Scatter};
use shaders::{Shader, RayTracer, NormalShader, DepthShader, ScatterShader};

fn main() {
    // image and camera
    let height = 500;
    let width = 500;
    let num_samples: usize = 4;
    let image_data = ImageData::new(width as usize, height as usize, num_samples);

    // materials
    let metal = MetalStruct::new(Color::white(), 0.0);
    let dielectric = DielectricStruct::new(1.4);
    let red_lambertian = LambertianStruct::new(Color::red());
    let white_lambertian = LambertianStruct::new(Color::white());

    // objects
    let small_r = 2.0;
    let box_spacing = 3.0*small_r;
    let sphere_center = 0.5*box_spacing*Vec3::new(1.0, -2.0, -1.0) + small_r*Vec3::y_hat();
    let sphere_metal = SphereStruct::new(
            sphere_center,
            small_r,
            &metal
        );
    let sphere_red = SphereStruct::new(
            sphere_center + box_spacing*Vec3::new(0.0, 1.0, 1.0),
            small_r,
            &red_lambertian
        );
    let sphere_glass = SphereStruct::new(
            sphere_center + box_spacing*Vec3::new(-1.0, 1.0, 0.0),
            small_r,
            &dielectric
        );
    
    let box1 = CuboidStruct::new(
        Vec3::new( 0.0*box_spacing, -1.0*box_spacing,  0.0*box_spacing),
        Vec3::new(-1.0*box_spacing,  0.0*box_spacing, -1.0*box_spacing),
        &white_lambertian
    );
    let box2 = CuboidStruct::new(
        Vec3::new( 0.0*box_spacing, -1.0*box_spacing,  0.0*box_spacing),
        Vec3::new( 1.0*box_spacing,  0.0*box_spacing,  1.0*box_spacing),
        &white_lambertian
    );
    let box3 = CuboidStruct::new(
        Vec3::new( 0.0*box_spacing, -1.0*box_spacing,  0.0*box_spacing),
        Vec3::new( 1.0*box_spacing, -2.0*box_spacing, -1.0*box_spacing),
        &white_lambertian
    );
    
    let mut world = HittableListStruct::new()
        .push(&sphere_metal)
        .push(&sphere_red)
        .push(&sphere_glass)
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
        Color::new(0.1, 0.5, 0.7), Color::white()
    );
    let world_size = world.list.len();
    let bvh = BVHNodeStruct::new(&mut world, 0, world_size);

    camera.render(&bvh, background);


    camera.image.write(String::from("ray_tracer.ppm")).unwrap();
    camera.normal_image.write(String::from("normals.ppm")).unwrap();
    camera.scatter_image.write(String::from("scatter.ppm")).unwrap();
    camera.depth_image.write(String::from("depth.ppm")).unwrap();
}

fn random_float(min: f64, max: f64) -> f64 {
    let mut rng = rand::thread_rng();
    return rng.gen_range(min..max);
}

