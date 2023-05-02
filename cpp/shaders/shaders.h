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
Color ray_tracing_shader(const Ray &r, HittableList &world, std::shared_ptr<HittableList>& lights, Background &background, int depth)
{
    
    HitRecord rec;
    // max depth reached
    if (depth <= 0) {
        return Color(0, 0, 0);
    }

    // background
    if (!world.hit(r, 0.001f, 100.0f, rec)) {
        return background.get_color(r);
    }

    // hit something
    Ray scattered;
    Color attenuation;
    Color emitted = rec.getMaterial()->emitted(rec.getU(), rec.getV(), rec.getHitPoint());
    double pdf_value;
    Color albedo;

    if (!rec.getMaterial()->scatter(r, rec, albedo, scattered, pdf_value, lights)) {
        return emitted;
    }
    
    return emitted + albedo * ray_tracing_shader(scattered, world, lights, background, depth - 1) / pdf_value;
}


Color scattering_shader(const Ray &r, HittableList &world, std::shared_ptr<HittableList>& lights, Background &background, int depth)
{
    HitRecord rec = HitRecord();
    if (world.hit(r, 0.001f, 100.0f, rec))
    {
        Ray scattered;
        Color attenuation;
        double pdf;  
        if (rec.getMaterial()->scatter(r, rec, attenuation, scattered, pdf)) 
        {
            return scattered.direction*0.5 + Vec3(0.5,0.5, 0.5);
        }
    }
    
    return background.get_color(r);
}


Color normal_shader(const Ray &r, HittableList &world, std::shared_ptr<HittableList>& lights, Background &background, int depth)
{
    HitRecord rec;
    if (!world.hit(r, 0.001f, 100.0f, rec))
    {
        return background.get_color(r);
    }
    
    Vec3 normal = rec.isFrontFace(r) ? rec.getNormal() : -rec.getNormal();
    return normal*0.5 + Vec3(0.5, 0.5, 0.5);
}
