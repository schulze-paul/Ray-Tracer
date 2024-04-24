use std::fs;

use crate::{Vec3, Color, Ray};
use crate::{dot, cross};


#[derive(Debug, Clone)]
pub struct Camera {
    pub image: ImageData,
    pub normal_image: ImageData,
    pub scatter_image: ImageData,
    pub depth_image: ImageData,
    pub look_from: Vec3,
    pub look_at: Vec3,
    lower_left_corner: Vec3,
    pub horizontal: Vec3,
    pub vertical: Vec3,
    pub u: Vec3, 
    pub v: Vec3, 
    pub w: Vec3,
    lens_radius: f64,
    viewport_width: f64,
    viewport_height: f64,
    t_min: f64,
    t_max: f64,
    aspect_ratio: f64,
    focus_dist: f64,
    pub vfov: f64
}

impl Camera {
    pub fn new(image_data: ImageData) -> Camera{
        let mut camera = Camera {
            image: image_data.clone(),
            normal_image: image_data.clone(),
            scatter_image: image_data.clone(),
            depth_image: image_data.clone(),

            look_from: Vec3::x_hat(),
            look_at: Vec3::zero(),
            lower_left_corner: Vec3::zero(),
            horizontal: Vec3::zero(),
            vertical: Vec3::zero(),
            u: Vec3::zero(),
            v: Vec3::zero(),
            w: Vec3::zero(),
            lens_radius: 0.1,
            viewport_width: 0.0,
            viewport_height: 0.0,
            t_min: 0.0,
            t_max: 1.0,
            aspect_ratio: 1.0,
            focus_dist: 10.0,
            vfov: 50.0,
        };
        camera.set_configuration();
        return camera;
    }
    pub fn get_ray(&self, u: f64, v: f64) -> Ray {
        // TODO set random function
        let random_in_unit_disk = self.lens_radius * Vec3::random_in_unit_sphere();
        let offset = self.u * random_in_unit_disk.x() + self.v * random_in_unit_disk.y();
        return Ray::new(
            self.look_from + offset,
            self.lower_left_corner + u * self.horizontal + v * self.vertical - self.look_from - offset,
        );

    }

    fn set_configuration(&mut self) {
        let theta = f64::to_radians(self.vfov);
        let h = f64::tan(theta/2.0);
        self.viewport_height = 2.0 * h;
        self.viewport_width = self.aspect_ratio*self.viewport_height;
        
        self.w = (self.look_from-self.look_at).unit();
        self.u = cross(Vec3::new(0.0,1.0,0.0), self.w).unit();
        self.v = cross(self.w, self.u);
        
        self.horizontal = self.u * self.focus_dist * self.viewport_width;
        self.vertical   = self.v * self.focus_dist * self.viewport_height;
        self.lower_left_corner = self.look_from - self.horizontal / 2.0 - self.vertical / 2.0 - self.focus_dist * self.w;
    }

    pub fn set_vfov(mut self, vfov: f64) -> Camera {
        self.vfov = vfov;
        self.set_configuration();
        return self
    }
    pub fn set_aspect_ratio(mut self, aspect_ratio: f64) -> Camera {
        self.aspect_ratio = aspect_ratio;
        self.set_configuration();
        return self
    }
    pub fn set_aperture(mut self, aperture: f64) -> Camera {
        self.lens_radius = aperture/2.0;
        return self
    }
    pub fn set_focus_dist(mut self, focus_dist: f64) -> Camera {
        self.focus_dist = focus_dist;
        self.set_configuration();
        return self
    }
    pub fn focus_on_look_at(mut self) -> Camera {
        self.focus_dist = (
            self.look_at - self.look_from).length();
        self.set_configuration();
        return self
    }

    pub fn look_from(mut self, look_from: Vec3) -> Camera {
        self.look_from = look_from;
        self.set_configuration();
        return self
    }
    pub fn look_at(mut self, look_at: Vec3) -> Camera {
        self.look_at = look_at;
        self.set_configuration();
        return self
    }
    pub fn set_times(mut self, t_min: f64, t_max: f64) -> Camera {
        self.t_min = t_min;
        self.t_max = t_max;
        return self
    }
}

#[derive(Debug, Clone)]
pub struct ImageData {
    pixels: Vec<Color>,
    pub width: usize,
    pub height: usize,
    num_samples: u16
}

impl ImageData {
    pub fn new(width: usize, height: usize, num_samples: usize) -> ImageData{
        ImageData {
            pixels: vec![Color::black(); width*height],
            width,
            height,
            num_samples: num_samples.try_into().expect(""),
        }
    }
    pub fn scale(&mut self, r_min: f64, r_max: f64, new_min: f64, new_max: f64) {
        for i in 0..self.pixels.len() {
            if self.pixels[i] != Color::black() {
                self.pixels[i] = 
                    (self.pixels[i] - Color::white()*r_min)
                    / (r_max - r_min) 
                    * (new_max - new_min) 
                    + Color::white()*new_min;
            }
        }
    }
    pub fn set(&mut self, u: usize, v: usize, pixel_data: Color) {
        self.pixels[self.width*u + v] = pixel_data;
    }
    pub fn add(&mut self, u: usize, v: usize, pixel_data: Color) {
        self.pixels[self.width*u + v] += pixel_data;
    }
    pub fn get(&self, u: usize, v: usize) -> Color {
        return self.pixels[self.width*u + v];
    }
    pub fn write(self, path: String) -> Result<(), std::io::Error> {
        let max_value: f64 = 255.999;
        let mut color: Color;
        let mut out_string: String = format!("P3\n{} {}\n{}\n", self.width, self.height, max_value.floor() as i32); 
        for v_index in (0..self.height).rev() {
            for u_index in 0..self.width {
                color = self.get(u_index, v_index);
                out_string += &self.get_color_string(color);
            }
        }
        return fs::write(path, out_string);
    }
    fn get_color_string(&self, color: Color) -> String {
        
        let max_value: f64 = 255.999;
        let r = clamp((max_value * color.r() / f64::from(self.num_samples)).round() as i32, 0, 255);
        let g = clamp((max_value * color.g() / f64::from(self.num_samples)).round() as i32, 0, 255);
        let b = clamp((max_value * color.b() / f64::from(self.num_samples)).round() as i32, 0, 255);
        return format!("{} {} {}\n", r, g, b) 
    }
}


fn clamp(num: i32, min: i32, max: i32) -> i32{
    if num > min && num < max {
        return num;
    }
    else if num > max {
        return max;
    }
    else {
        return min;
    }
}
