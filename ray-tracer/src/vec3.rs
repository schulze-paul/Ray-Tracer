use std::ops::{Add, AddAssign, Div, DivAssign, Index, IndexMut, Mul, MulAssign, Neg, Sub, SubAssign};
use std::cmp::PartialEq;

#[derive(Debug, Clone, Copy)]
pub struct Vec3 {
    e: [f64;3],
}

impl Vec3 {
    pub fn new(x: f64, y: f64, z: f64) -> Vec3 {
        Vec3 {
            e: [x, y, z]
        }
    }
    pub fn zero() -> Vec3 {
        Vec3{e: [0.0,0.0,0.0]}
    }
    pub fn one() -> Vec3 {
        Vec3{e: [0.0,0.0,0.0]}
    }
    pub fn x(&self) -> f64 {
        self.e[0]
    }
    pub fn y(&self) -> f64 {
        self.e[1]
    }
    pub fn z(&self) -> f64 {
        self.e[2]
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
    pub fn length_squared(&self) -> f64 {
        self.e.into_iter().map(|x| x*x).sum::<f64>()
    }
    pub fn length(&self) -> f64 {
        self.e.into_iter().map(|x| x*x).sum::<f64>().sqrt()
    }
    pub fn sum(&self) -> f64 {
        self.x() + self.y() + self.z()
    }
    pub fn is_near_zero(&self)-> bool{
        let s = 1e-8;
        self.x() < s && self.y() < s && self.z() < s
    }
    pub fn unit(&self) -> Vec3 {
        self.clone()/self.length()
    }
}

pub fn dot(a: Vec3, b: Vec3) -> f64 {
    a.x()*b.x() + a.y()*b.y() + a.z()*b.z()
}
pub fn cross(a: Vec3, b: Vec3) -> Vec3 {
    Vec3::new(
        a.y()*b.z() - a.z()*b.y(),
        a.z()*b.x() - a.x()*b.z(),
        a.x()*b.y() - a.y()*b.x(),
    )
}

impl Index<usize> for Vec3 {
    type Output = f64;
    fn index<'a>(&'a self, i: usize) -> &'a f64{
        &self.e[i]
    }
}

impl IndexMut<usize> for Vec3 {
    fn index_mut<'a>(&'a mut self, i: usize) -> &'a mut f64 {
        &mut self.e[i]
    }
}

impl Sub for Vec3{
    type Output = Vec3;
    fn sub(self, rhs: Vec3) -> Vec3 {
        Vec3::new(
            self[0]-rhs[0],
            self[1]-rhs[1],
            self[2]-rhs[2]
        )
    }
}
impl SubAssign for Vec3 {
    fn sub_assign(&mut self, rhs: Self) {
        self[0] -= rhs[0];
        self[1] -= rhs[1];
        self[2] -= rhs[2];
    }
}

impl Add for Vec3{
    type Output = Vec3;

    fn add(self, rhs: Vec3) -> Vec3 {
        Vec3::new(self[0]+rhs[0],self[1]+rhs[1],self[2]+rhs[2])
    }
}
impl AddAssign for Vec3 {
    fn add_assign(&mut self, rhs: Self) {
        self[0] += rhs[0];
        self[1] += rhs[1];
        self[2] += rhs[2];
    }
}

impl Div<f64> for Vec3 {
    type Output = Vec3;
    fn div(self, rhs: f64) -> Self::Output {
        Vec3::new(
            self[0]/rhs,
            self[1]/rhs,
            self[2]/rhs
        )
    }
}
impl DivAssign<f64> for Vec3 {
    fn div_assign(&mut self, rhs: f64) {
        self[0] /= rhs;
        self[1] /= rhs;
        self[2] /= rhs;
    }
}

impl Mul<Vec3> for f64 {
    type Output = Vec3;
    fn mul(self, rhs: Vec3) -> Self::Output {
        Vec3::new(
            self*rhs.x(),
            self*rhs.y(),
            self*rhs.z()
        )
    }
}

impl Mul<f64> for Vec3 {
    type Output = Vec3;
    fn mul(self, rhs: f64) -> Self::Output {
        Vec3::new(
            self.x()*rhs,
            self.y()*rhs,
            self.z()*rhs
        )
    }
}

impl MulAssign<f64> for Vec3 {
    fn mul_assign(&mut self, rhs: f64) {
        self[0] *= rhs;
        self[1] *= rhs;
        self[2] *= rhs;
    }
}

impl Neg for Vec3 {
    type Output = Vec3;
    fn neg(self) -> Self::Output {
        Vec3::new(
            -self[0],
            -self[1],
            -self[2]
        )
    }
}

impl PartialEq for Vec3 {
    fn eq(&self, other: &Vec3) -> bool {
        self.x() == other.x() &&
        self.y() == other.y() &&
        self.z() == other.z()
    }
}
