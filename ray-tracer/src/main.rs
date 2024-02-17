mod ray;
mod vec3;
mod hit_record;
mod hittables;

use vec3::Vec3;

fn main() {
    let vector = Vec3::new(1.0,1.0,1.0);

    println!("Hello, world!");
    print!("{}", vector.length())
}
