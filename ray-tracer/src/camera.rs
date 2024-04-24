
use crate::{DepthShader, Hit, NormalShader, Ray, RayTracer, ScatterShader, Shader, Vec3, GradientBackground, ImageData};
use crate::cross;


#[derive(Debug, Clone)]
pub struct Camera {
    pub image: ImageData,
    pub normal_image: ImageData,
    pub scatter_image: ImageData,
    pub depth_image: ImageData,
    num_samples: usize,
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
            num_samples: 1,

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
    pub fn with_num_samples(mut self, num_samples: usize) -> Camera {
        self.num_samples = num_samples;
        return self;
    }

    pub fn with_vfov(mut self, vfov: f64) -> Camera {
        self.vfov = vfov;
        self.set_configuration();
        return self
    }
    pub fn with_aspect_ratio(mut self, aspect_ratio: f64) -> Camera {
        self.aspect_ratio = aspect_ratio;
        self.set_configuration();
        return self
    }
    pub fn with_aperture(mut self, aperture: f64) -> Camera {
        self.lens_radius = aperture/2.0;
        return self
    }
    pub fn with_focus_dist(mut self, focus_dist: f64) -> Camera {
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
    pub fn with_times(mut self, t_min: f64, t_max: f64) -> Camera {
        self.t_min = t_min;
        self.t_max = t_max;
        return self
    }
    pub fn render(&mut self, world: &dyn Hit, background: GradientBackground) {

        let mut ray_tracer = RayTracer{};
        let mut normal_shader = NormalShader{};
        let mut scatter_shader = ScatterShader{};
        let mut depth_shader = DepthShader::new();
        let mut progress = 0;
        let max_progress = self.num_samples*(self.image.width as usize)*(self.image.height as usize);
        for index_u in 0..self.image.height {
            for index_v in 0..self.image.width {
                let u: f64 = index_u as f64 / (self.image.width  - 1) as f64;
                let v: f64 = index_v as f64 / (self.image.height - 1) as f64;

                for i_samples in 0..self.num_samples {
                    progress += 1;
                    println!("{}%", (((1000*progress)/max_progress) as f64)/10.0);
                    
                    let ray_in = self.get_ray(u, v);

                    self.image.add(index_u, index_v, 
                        ray_tracer.get_color(ray_in, world, &background, 0)
                    );
                }
                let ray_in = self.get_ray(u, v);
                self.normal_image.add(index_u, index_v,
                    normal_shader.get_color(ray_in, world, &background, 0)
                );
                self.scatter_image.add(index_u, index_v,
                    scatter_shader.get_color(ray_in, world, &background, 0)
                );
                self.depth_image.add(index_u, index_v,
                    depth_shader.get_color(ray_in, world, &background, 0)
                );
            }
        }
        self.depth_image.scale(depth_shader.max_depth, depth_shader.min_depth, 0.2, 1.0);

    }
}

