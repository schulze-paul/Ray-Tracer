mod ray;
mod vec3;
mod color;
mod hit_record;
mod hittables;
mod material;
mod camera;
mod background;

use std::{f64, usize};
use rand::Rng;

use hit_record::{HitType, HitRecord, ScatterRecord};
use ray::Ray;
use vec3::{Vec3, dot, cross};
use color::Color;
use camera::{Camera, ImageData};
use hittables::{Hit, HittableListStruct, SphereStruct, CuboidStruct, BVHNodeStruct};
use background::GradientBackground;
use material::{DielectricStruct, LambertianStruct, MetalStruct, Scatter};

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


    match camera.image.write(String::from("ray_tracer.ppm")) {
        Ok(_) => {println!("Ok");},
        Err(e) => {println!("{}", e);},
    };
    match camera.normal_image.write(String::from("normals.ppm")) {
        Ok(_) => {println!("Ok");},
        Err(e) => {println!("{}", e);},
    };
    match camera.scatter_image.write(String::from("scatter.ppm")) {
        Ok(_) => {println!("Ok");},
        Err(e) => {println!("{}", e);},
    };
    match camera.depth_image.write(String::from("depth.ppm")) {
        Ok(_) => {println!("Ok");},
        Err(e) => {println!("{}", e);},
    };
}

pub trait Shader {
    fn get_color<'a>(&mut self, ray_in: Ray, world: &'a dyn for <'b> Hit, background: &GradientBackground, depth: i32) -> Color;
}

pub struct RayTracer{}
impl Shader for RayTracer {
    fn get_color<'a>(&mut self, ray_in: Ray, world: &'a dyn for <'b> Hit, background: &GradientBackground, depth: i32) -> Color {
        let max_depth = 8;
        if depth >= max_depth {
            return Color::black();
        }
        let hit_record = world.hit(&ray_in, [0.001,1e20]);

        match hit_record {
            HitType::Hit(h) => {
                match h.material {
                    None => return background.get_color(&ray_in),
                    Some(m) => {
                        let scatter_rec = m.scatter(&ray_in, &h);
                        let colors: Vec<Color> = scatter_rec.probabilities.iter()
                            .zip(scatter_rec.scattered.iter())
                            .map(|(p, s)| *p * m.attenuation() * self.get_color(*s, world, background, depth+1))
                            .collect();
                        let mut color = Color::black();
                        for c in colors {
                            color += c;
                        }
                        return color
                    }
                }
            }
            _ => return background.get_color(&ray_in),
        }
    }

}

pub struct NormalShader{}
impl Shader for NormalShader{
    fn get_color<'a>(&mut self, ray_in: Ray, world: &'a dyn for <'b> Hit, _background: &GradientBackground, _depth: i32) -> Color {
        let hit_record = world.hit(&ray_in, [0.001, 1e20]);
        match hit_record {
            HitType::Hit(h) => return Color::from_vector(h.normal),
            _ => return Color::black()
        }
    }
}

pub struct ScatterShader{}
impl Shader for ScatterShader{
    fn get_color<'a>(&mut self, ray_in: Ray, world: &'a dyn for <'b> Hit, _background: &GradientBackground, _depth: i32) -> Color {
        let hit_record = world.hit(&ray_in, [0.001,1e20]);
        match hit_record {
            HitType::Hit(h) => {
                match h.material {
                    None => return Color::black(),
                    Some(m) => {
                        let scatter_rec = m.scatter(&ray_in, &h);
                        let colors: Vec<Color> = scatter_rec.scattered.iter()
                            .map(|s| Color::from_vector(s.direction))
                            .collect();
                        let mut color = Color::black();
                        for c in colors {
                            color += c;
                        }
                        return color
                    }
                }
            }
            _ => return Color::black(),
        }
    }
}

struct DepthShader{
    min_depth: Option<f64>,
    max_depth: Option<f64>
}
impl DepthShader {
    fn new() -> DepthShader {
        return DepthShader{min_depth: None, max_depth: None}
    }
    fn register(&mut self, t_hit: f64) {
        match self.min_depth.zip(self.max_depth) {
            Some((min, max)) => {
                if t_hit < min {
                    self.min_depth = Some(t_hit);
                }
                if t_hit > max {
                    self.max_depth = Some(t_hit);
                }
            }
            _ => {
                self.min_depth = Some(t_hit);
                self.max_depth = Some(t_hit);
            }
        }
    }
}

impl Shader for DepthShader{
    fn get_color<'a>(&mut self, ray_in: Ray, world: &'a dyn for <'b> Hit, _background: &GradientBackground, _depth: i32) -> Color {

       let hit_record = world.hit(&ray_in, [0.001, 1e20]);
       match hit_record {
            HitType::Hit(h) => {
                self.register(h.t_hit);
                return Color::grey(f64::exp(-h.t_hit))
            },
            _ => return Color::black()
       }
    }
}
fn random_float(min: f64, max: f64) -> f64 {
    let mut rng = rand::thread_rng();
    return rng.gen_range(min..max);
}


