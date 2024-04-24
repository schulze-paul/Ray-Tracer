use crate::{Vec3, Ray, Hit, Scatter, HitType, HitRecord, BoundingBoxStruct};
use crate::dot;

#[derive(Clone)]
pub enum Rectangle<'a> {
    XY(RectangleStruct<'a>),
    XZ(RectangleStruct<'a>),
    YZ(RectangleStruct<'a>),
}

#[derive(Clone)]
struct RectangleStruct<'a>{
    a_min: f64,
    a_max: f64,
    b_min: f64,
    b_max: f64,
    k: f64,
    material: &'a dyn Scatter
}

impl RectangleStruct<'_> {
    fn new(a_min: f64, a_max: f64, b_min: f64, b_max: f64, k: f64, material: &dyn Scatter) -> RectangleStruct {
        return RectangleStruct{a_min, a_max, b_min, b_max, k, material}
    }
}

impl <'a>Rectangle<'_> {
    pub fn new_xy(x0: f64, x1: f64, y0: f64, y1: f64, k: f64, material: &'a dyn Scatter) -> Rectangle<'a> {
        let x_min = f64::min(x0, x1);
        let x_max = f64::max(x0, x1);
        let y_min = f64::min(y0, y1);
        let y_max = f64::max(y0, y1);
        Rectangle::XY(RectangleStruct::new(x_min,x_max,y_min, y_max, k, material))
    }
    pub fn new_xz(x0: f64, x1: f64, z0: f64, z1: f64, k: f64, material: &'a dyn Scatter) -> Rectangle<'a> {
        let x_min = f64::min(x0, x1);
        let x_max = f64::max(x0, x1);
        let z_min = f64::min(z0, z1);
        let z_max = f64::max(z0, z1);
        Rectangle::XZ(RectangleStruct::new(x_min,x_max,z_min, z_max, k, material))
    }
    pub fn new_yz(y0: f64, y1: f64, z0: f64, z1: f64, k: f64, material: &'a dyn Scatter) -> Rectangle<'a> {
        let y_min = f64::min(y0, y1);
        let y_max = f64::max(y0, y1);
        let z_min = f64::min(z0, z1);
        let z_max = f64::max(z0, z1);
        Rectangle::YZ(RectangleStruct::new(y_min,y_max,z_min, z_max, k, material))
    }
}

impl<'a> Hit for Rectangle<'a> {
    fn hit(&self, ray: &Ray, range: [f64;2]) -> HitType {
        let dim_a;
        let dim_b;
        let dim_c;
        let rect: &RectangleStruct;
        match self {
            Self::XY(r) => {
                dim_a = 0;
                dim_b = 1;
                dim_c = 2;
                rect = r;
            }
            Self::XZ(r) => {
                dim_a = 0;
                dim_b = 2;
                dim_c = 1;
                rect = r;
            }
            Self::YZ(r) => {
                dim_a = 1;
                dim_b = 2;
                dim_c = 0;
                rect = r;
            }
        }
        let t = (rect.k - ray.origin[dim_c]) / ray.direction[dim_c];
        if t < range[0] || t > range[1] {
            return HitType::None
        }
        let a = ray.origin[dim_a] + t * ray.direction[dim_a];
        let b = ray.origin[dim_b] + t * ray.direction[dim_b];
        if a < rect.a_min || a > rect.a_max || b < rect.b_min || b > rect.b_max {
            return HitType::None;
        }
        let mut normal = Vec3::zero();
        normal[dim_c] = 1.0;
        if dot(ray.direction, normal) > 0.0 {
            normal = -normal;
        }
        let hit_point = ray.at(t);
        return HitType::Hit(
            HitRecord::new(t, hit_point, ray.direction, normal)
                .with_material(rect.material)
        );
    }
    fn bounding_volume(&self) -> Option<BoundingBoxStruct> {
        let dim_a;
        let dim_b;
        let dim_c;
        let rect: &RectangleStruct;
        match self {
            Self::XY(r) => {
                dim_a = 0;
                dim_b = 1;
                dim_c = 2;
                rect = r;
            }
            Self::XZ(r) => {
                dim_a = 0;
                dim_b = 2;
                dim_c = 1;
                rect = r;
            }
            Self::YZ(r) => {
                dim_a = 1;
                dim_b = 2;
                dim_c = 0;
                rect = r;
            }
        }
        let mut v_min = Vec3::zero();
        v_min[dim_a] = rect.a_min;
        v_min[dim_b] = rect.b_min;
        v_min[dim_c] = rect.k-0.0001;
        let mut v_max = Vec3::zero();
        v_max[dim_a] = rect.a_max;
        v_max[dim_b] = rect.b_max;
        v_max[dim_c] = rect.k+0.0001;
        return Some(BoundingBoxStruct::new(v_min, v_max));
    }
}

#[derive(Clone)]
pub struct CuboidStruct<'a> {
    sides: [Rectangle<'a>; 6],
    v_max: Vec3,
    v_min: Vec3,
}

impl <'a>CuboidStruct<'_> {
    pub fn new(p0: Vec3, p1: Vec3, material: &'a dyn Scatter) -> CuboidStruct<'a> {

        let mut v_min = Vec3::zero();
        let mut v_max = Vec3::zero();
        for i in 0..3 {
            v_min[i] = f64::min(p0[i], p1[i]);
            v_max[i] = f64::max(p0[i], p1[i]);
        }

        let sides = [
            Rectangle::new_xy(v_min.x(), v_max.x(), v_min.y(), v_max.y(), v_min.z(), material),
            Rectangle::new_xy(v_min.x(), v_max.x(), v_min.y(), v_max.y(), v_max.z(), material),
            Rectangle::new_xz(v_min.x(), v_max.x(), v_min.z(), v_max.z(), v_min.y(), material),
            Rectangle::new_xz(v_min.x(), v_max.x(), v_min.z(), v_max.z(), v_max.y(), material),
            Rectangle::new_yz(v_min.y(), v_max.y(), v_min.z(), v_max.z(), v_min.x(), material),
            Rectangle::new_yz(v_min.y(), v_max.y(), v_min.z(), v_max.z(), v_max.x(), material),
        ];
        return CuboidStruct{sides, v_min, v_max};
    }
}
impl<'a> Hit for CuboidStruct<'a> {
    fn hit(&self, ray: &Ray, range: [f64;2]) -> HitType {
        let mut closest_hit_record = HitType::None;
        for hittable in &self.sides {
            let hit_record = hittable.hit(ray, range);
            match hit_record {
                HitType::Hit(h) => {
                    match closest_hit_record {
                        HitType::Hit(ref c) => {
                            if h.t_hit < c.t_hit {
                                closest_hit_record = HitType::Hit(h) 
                            }
                        }
                        _ => closest_hit_record = HitType::Hit(h),
                    }
                }
                _ => continue,
            }
        }
        return closest_hit_record;
    }
    fn bounding_volume(&self) -> Option<BoundingBoxStruct> {
        return Some(
            BoundingBoxStruct::new(
                self.v_min - 0.0001*Vec3::ones(), 
                self.v_max+0.0001*Vec3::ones()
            ));
    }
}
