use crate::{Vec3, Ray, Color, Hit, GradientBackground, Interval};

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
        let hit_record = world.hit(&ray_in, Interval::new(0.001,1e20));

        match hit_record {
            Some(h) => {
                match h.material {
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
                        return color + m.emittance();
                    }
                    None => return background.get_color(&ray_in),
                }
            }
            _ => return background.get_color(&ray_in),
        }
    }

}

pub struct NormalShader{}
impl Shader for NormalShader{
    fn get_color<'a>(&mut self, ray_in: Ray, world: &'a dyn for <'b> Hit, _background: &GradientBackground, _depth: i32) -> Color {
        let hit_record = world.hit(&ray_in, Interval::new(0.001, 1e20));
        match hit_record {
            Some(h) => return Color::from_vector(h.normal),
            None => return Color::black()
        }
    }
}

pub struct ScatterShader{}
impl Shader for ScatterShader{
    fn get_color<'a>(&mut self, ray_in: Ray, world: &'a dyn for <'b> Hit, _background: &GradientBackground, _depth: i32) -> Color {
        let hit_record = world.hit(&ray_in, Interval::new(0.001, 1e20));
        match hit_record {
            Some(h) => {
                match h.material {
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
                    None => return Color::black(),
                }
            }
            _ => return Color::black(),
        }
    }
}

pub struct DepthShader{
    pub min_depth: f64,
    pub max_depth: f64,
}
impl DepthShader {
    pub fn new() -> DepthShader {
        return DepthShader{min_depth: f64::MAX, max_depth: f64::MIN}
    }
    fn register(&mut self, t_hit: f64) {
        if t_hit < self.min_depth {
            self.min_depth = t_hit;
        }
        if t_hit > self.max_depth {
            self.max_depth = t_hit;
        }
    }
}

impl Shader for DepthShader{
    fn get_color<'a>(&mut self, ray_in: Ray, world: &'a dyn for <'b> Hit, _background: &GradientBackground, _depth: i32) -> Color {

       let hit_record = world.hit(&ray_in, Interval::new(0.001,1e20));
       match hit_record {
            Some(h) => {
                self.register(h.t_hit);
                return Color::grey(h.t_hit)
            },
            None => return Color::black()
       }
    }
}

