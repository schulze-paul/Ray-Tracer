#include <memory>
#include <vector>
#include <cmath>

#include "color.h"
#include "ray.h"
#include "hit_record.h"
#include "hittable_list.h"
#include "background.h"
#include "pdf.h"


Color ray_tracing_shader(const Ray &r, HittableList &world, std::shared_ptr<HittableList>& lights, Background &background, int depth)
{
    HitRecord rec;
    // max depth reached
    if (depth <= 0)
        return Color(0, 0, 0);
    
    // background
    if (!world.hit(r, 0.001f, 100.0f, rec))
        return background.get_color(r);

    // hit something
    Ray scattered;
    Color attenuation;
    Color emitted = rec.getMaterial()->emitted(rec.getU(), rec.getV(), rec.getHitPoint());
    double pdf_value;
    Color albedo;
    if (!rec.getMaterial()->scatter(r, rec, albedo, scattered, pdf_value))
        return emitted;

    if (!rec.getMaterial()->isLambertian())
        return emitted + albedo * ray_tracing_shader(scattered, world, lights, background, depth - 1);

    auto light_pdf = std::make_shared<HittablePDF>(lights, rec.getHitPoint(), r.get_time());
    auto cosine_pdf = std::make_shared<CosinePDF>(rec.getNormal());
    MixturePDF mixture_pdf = MixturePDF(light_pdf, cosine_pdf);
    
    std::vector<double> pdf_values;
    std::vector<Vec3> directions;
    mixture_pdf.generate(directions, pdf_values);
    
    Color color = Color(0, 0, 0);
    for (int i = 0; i < directions.size(); i++)
    {
        scattered = Ray(rec.getHitPoint(), directions[0], r.get_time());
        color +=emitted + albedo * ray_tracing_shader(scattered, world, lights, background, depth - 1) / pdf_values[i]);
    }
    return color / directions.size();
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
