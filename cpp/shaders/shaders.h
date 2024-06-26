#include <memory>
#include <vector>
#include <cmath>

#include "color.h"
#include "ray.h"
#include "hit_record.h"
#include "hittable_list.h"
#include "background.h"
#include "pdf.h"

/**
 * Ray tracing shader
 * @param r: ray to trace
 * @param world: hittable objects
 * @param lights: lights
 * @param background: background for the scene
 * @param depth: max depth for recursion 
 */   
Color ray_tracing_shader(const Ray &ray_in, HittableList &world, Background &background, int depth)
{
    
    if (depth <= 0) {
        // max depth reached
        return Color(0, 0, 0);
    }

    HitRecord hit_rec;
    if (!world.hit(ray_in, 0.001f, infinity, hit_rec)) {
        // background
        return background.get_color(ray_in);
    }

    ScatterRecord scatter_record;
    Color emitted = hit_rec.get_material()->emitted(hit_rec.u, hit_rec.v, hit_rec.get_hit_point());
    if (!hit_rec.get_material()->scatter(ray_in, hit_rec, scatter_record)) {
        // no scatter
        return emitted;
    }

    if (scatter_record.is_specular) {
        // specular reflection or refraction
        return scatter_record.attenuation * ray_tracing_shader(scatter_record.specular_ray, world, background, depth - 1);
    }

    // diffuse
    Vec3 direction;
    double pdf_val;
    scatter_record.pdf->generate(direction, pdf_val);
    Ray scattered = Ray(hit_rec.get_hit_point(), direction, ray_in.get_time());
    return emitted + scatter_record.attenuation * hit_rec.get_material()->scattering_pdf(ray_in, hit_rec, scattered) * ray_tracing_shader(scattered, world, background, depth - 1) / pdf_val;

}


// Color scattering_shader(const Ray &ray_in, HittableList &world, std::shared_ptr<HittableList>& lights, Background &background, int depth)
// {
//     HitRecord hit_rec = HitRecord();
//     if (world.hit(ray_in, 0.001f, infinity, hit_rec))
//     {
//         ScatterRecord scatter_rec;
//         if (rec.get_material()->scatter(ray_in, hit_rec, scatter_rec)) 
//         {
//             return scatter_rec.*0.5 + Vec3(0.5,0.5, 0.5);
//         }
//     }
    
//     return background.get_color(r);
// }


// Color normal_shader(const Ray &r, HittableList &world, std::shared_ptr<HittableList>& lights, Background &background, int depth)
// {
//     HitRecord rec;
//     if (!world.hit(r, 0.001f, 100.0f, rec))
//     {
//         return background.get_color(r);
//     }
    
//     Vec3 normal = rec.is_front_face(r) ? rec.get_normal() : -rec.get_normal();
//     return normal*0.5 + Vec3(0.5, 0.5, 0.5);
// }
