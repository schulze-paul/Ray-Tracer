use crate::{Ray, Vec3, Color};


pub struct GradientBackground{
    color1: Color,
    color2: Color
}

impl GradientBackground{
    pub fn new(color1: Color, color2: Color) -> GradientBackground {
        GradientBackground{color1, color2}
    }
    pub fn get_color(&self, ray_in: &Ray) -> Color {
        let interpolation_val = 0.5*(ray_in.direction.unit().y() + 1.0);
        return (1.0 - interpolation_val)*self.color1 + interpolation_val*self.color2;
    }
}


