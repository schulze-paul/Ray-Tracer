mod ray;
mod vec3;
mod hit_record;
mod hittables;
mod camera;

use std::f64;

use hit_record::HitRecord;
use vec3::Vec3;
use Vec3 as Color;
use crate::camera::{Camera, ImageData};
use hittables::{Hittable, SphereStruct, Hit};

fn main() {
    let width = 4;
    let height = 4;
    let image_data = ImageData::new(width, height);

    let mut camera = Camera::new(image_data);
    camera.look_from(Vec3::new(0.0, 0.0, 0.0));
    let sphere = Hittable::Sphere(
        SphereStruct::new(
            Vec3::new(8.0,0.0,0.0), 1.0
        )
    );
    if let Hittable::Sphere(spherestruct) = &sphere {
        camera.look_at(spherestruct.center);
    };
    println!("{:?}", &camera.image_data);

    for index_u in 0..camera.image_data.width {
        println!("u:{}", index_u);
        for index_v in 0..camera.image_data.height {
            println!("v: {}", index_v);
            let u: f64 = index_u as f64 / (camera.image_data.width  - 1) as f64;
            let v: f64 = index_v as f64 / (camera.image_data.height - 1) as f64;
            
            let ray = camera.get_ray(u, v);
            let mut hit_record = HitRecord::new();
            if !sphere.hit(&ray, [0.0, 1.0], &mut hit_record) {
                continue;
            }
            camera.image_data.set(index_u, index_v, Color::new(1.0, 1.0, 1.0));
        }
    }
    match camera.image_data.write(String::from("first_test.ppm")) {
        Ok(_) => {println!("Ok");},
        Err(e) => {println!("{}", e);},
    };
}
