use std::fs;

use crate::Color;

#[derive(Debug, Clone)]
pub struct ImageData {
    pixels: Vec<Color>,
    pub width: usize,
    pub height: usize,
    num_samples: u16,
}

impl ImageData {
    pub fn new(width: usize, height: usize, num_samples: usize) -> ImageData {
        ImageData {
            pixels: vec![Color::black(); width * height],
            width,
            height,
            num_samples: num_samples.try_into().unwrap(),
        }
    }
    pub fn with_num_samples(mut self, num_samples: usize) -> ImageData {
        self.num_samples = num_samples.try_into().unwrap();
        return self;
    }
    pub fn scale(&mut self, r_min: f64, r_max: f64, new_min: f64, new_max: f64) {
        for i in 0..self.pixels.len() {
            if self.pixels[i] != Color::black() {
                self.pixels[i] = (self.pixels[i] - Color::white() * r_min) / (r_max - r_min)
                    * (new_max - new_min)
                    + Color::white() * new_min;
            }
        }
    }
    pub fn set(&mut self, u: usize, v: usize, pixel_data: Color) {
        self.pixels[self.width * u + v] = pixel_data;
    }
    pub fn add(&mut self, u: usize, v: usize, pixel_data: Color) {
        self.pixels[self.width * u + v] += pixel_data;
    }
    pub fn get(&self, u: usize, v: usize) -> Color {
        return self.pixels[self.width * u + v];
    }
    pub fn write(self, path: String) -> Result<(), std::io::Error> {
        let max_value: f64 = 255.999;
        let mut color: Color;
        let mut out_string: String = format!(
            "P3\n{} {}\n{}\n",
            self.width,
            self.height,
            max_value.floor() as i32
        );
        for v_index in (0..self.height).rev() {
            for u_index in 0..self.width {
                color = self.get(u_index, v_index);
                out_string += &self.get_color_string(color);
            }
        }
        return fs::write(path, out_string);
    }
    fn get_color_string(&self, color: Color) -> String {
        return format!(
            "{:.0} {:.0} {:.0}\n",
            get_ppm_value(color.r(), self.num_samples),
            get_ppm_value(color.g(), self.num_samples),
            get_ppm_value(color.b(), self.num_samples),
        );
    }
}

fn get_ppm_value(color_value: f64, num_samples: u16) -> f64 {
    let max_value: f64 = 255.999;
    let avg_color_all_samples = color_value / f64::from(num_samples);
    return clamp((max_value * avg_color_all_samples).round(), 0.0, 255.0);
}

fn clamp(num: f64, min: f64, max: f64) -> f64 {
    if num >= min && num <= max {
        return num;
    } else if num >= max {
        return max;
    } else {
        return min;
    }
}
