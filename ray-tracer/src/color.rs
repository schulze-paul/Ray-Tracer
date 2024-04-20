
use std::ops::{Add, AddAssign, Div, DivAssign, Index, IndexMut, Mul, MulAssign, Neg, Sub, SubAssign};
use std::cmp::PartialEq;

use crate::random_float;

#[derive(Debug, Clone, Copy)]
pub struct Color {
    e: [f64;3],
}

impl Color {
    pub fn new(r: f64, g: f64, b: f64) -> Color {
        Color {
            e: [r, g, b]
        }
    }
    pub fn black() -> Color {
        Color{e: [0.0,0.0,0.0]}
    }
    pub fn white() -> Color {
        Color{e: [1.0,1.0,1.0]}
    }
    pub fn grey(l: f64) -> Color {
        Color{e: [l,l,l]}
    }
    pub fn red() -> Color {
        Color{e: [1.0,0.0,0.0]}
    }
    pub fn green() -> Color {
        Color{e: [0.0,1.0,0.0]}
    }
    pub fn blue() -> Color {
        Color{e: [0.0,0.0,1.0]}
    }
    pub fn r(&self) -> f64 {
        self.e[0]
    }
    pub fn g(&self) -> f64 {
        self.e[1]
    }
    pub fn b(&self) -> f64 {
        self.e[2]
    }
    pub fn random(min: f64, max: f64) -> Color {
       return Color::new(
            random_float(min, max),
            random_float(min, max),
            random_float(min, max),
       ) 
    }
}

impl Index<usize> for Color {
    type Output = f64;
    fn index<'a>(&'a self, i: usize) -> &'a f64{
        return &self.e[i];
    }
}

impl IndexMut<usize> for Color {
    fn index_mut<'a>(&'a mut self, i: usize) -> &'a mut f64 {
        return &mut self.e[i];
    }
}

impl Sub for Color {
    type Output = Color;
    fn sub(self, rhs: Color) -> Color {
        return Color::new(
            self[0]-rhs[0],
            self[1]-rhs[1],
            self[2]-rhs[2]
        )
    }
}
impl SubAssign for Color {
    fn sub_assign(&mut self, rhs: Self) {
        self[0] -= rhs[0];
        self[1] -= rhs[1];
        self[2] -= rhs[2];
    }
}

impl Add for Color{
    type Output = Color;

    fn add(self, rhs: Color) -> Color {
        return Color::new(
            self[0]+rhs[0],
            self[1]+rhs[1],
            self[2]+rhs[2]);
    }
}
impl AddAssign for Color {
    fn add_assign(&mut self, rhs: Self) {
        self[0] += rhs[0];
        self[1] += rhs[1];
        self[2] += rhs[2];
    }
}

impl Div<f64> for Color {
    type Output = Color;
    fn div(self, rhs: f64) -> Self::Output {
        return Color::new(
            self[0]/rhs,
            self[1]/rhs,
            self[2]/rhs
        )
    }
}
impl DivAssign<f64> for Color {
    fn div_assign(&mut self, rhs: f64) {
        self[0] /= rhs;
        self[1] /= rhs;
        self[2] /= rhs;
    }
}
impl Mul<Color> for Color {
    type Output = Color;
    fn mul(self, rhs: Color) -> Self::Output {
        return Color::new(
            self.r()*rhs.r(),
            self.g()*rhs.g(),
            self.b()*rhs.b()
        );
    }
}

impl Mul<Color> for f64 {
    type Output = Color;
    fn mul(self, rhs: Color) -> Self::Output {
        return Color::new(
            self*rhs.r(),
            self*rhs.g(),
            self*rhs.b()
        );
    }
}

impl Mul<f64> for Color {
    type Output = Color;
    fn mul(self, rhs: f64) -> Self::Output {
        return Color::new(
            self.r()*rhs,
            self.g()*rhs,
            self.b()*rhs
        );
    }
}

impl MulAssign<f64> for Color {
    fn mul_assign(&mut self, rhs: f64) {
        self[0] *= rhs;
        self[1] *= rhs;
        self[2] *= rhs;
    }
}

impl Neg for Color {
    type Output = Color;
    fn neg(self) -> Self::Output {
        return Color::new(
            -self[0],
            -self[1],
            -self[2]
        );
    }
}

impl PartialEq for Color {
    fn eq(&self, other: &Color) -> bool {
        return self.r() == other.r() &&
               self.g() == other.g() &&
               self.b() == other.b()
    }
}
