use crate::{Vec3};
use crate::{cross};

#[derive(Clone, Copy, Debug)]
pub struct ONB {
    axis: [Vec3; 3],
}

impl ONB {
    pub fn from_w(normal: Vec3) -> ONB {
        let w = normal.unit();
        let a = if w.x().abs() > 0.9 {
            Vec3::y_hat()
        } else {
            Vec3::x_hat()
        };
        let v = cross(w, a).unit();
        let u = cross(w, v);
        return ONB{
            axis: [u, v, w],
        };
    }
    pub fn u(&self) -> Vec3 {
        return self.axis[0];
    }
    pub fn v(&self) -> Vec3 {
        return self.axis[1];
    }
    pub fn w(&self) -> Vec3 {
        return self.axis[2];
    }
    pub fn local(&self, a: f64, b: f64, c: f64) -> Vec3 {
        return self.u()*a + self.v()*b + self.w()*c;
    }
    pub fn local_from_vec(&self, a: Vec3) -> Vec3 {
        return self.u()*a.x() + self.v()*a.y() + self.w()*a.z();
    }
}
    
