#include "color.h"
#include "ray.h"
#include "hit_record.h"
#include "hittable_list.h"
#include "background.h"



Color ray_tracing_shader(const Ray &r, HittableList &world, Background &background, int depth)
{
    HitRecord rec;
    // max depth reached
    if (depth <= 0)
    {
        return Color(0, 0, 0);
    }

    // background
    if (!world.hit(r, 0.001f, 100.0f, rec))
    {
        return background.get_color(r);
    }

    // hit something
    Ray scattered;
    Color attenuation;
    Color emitted = rec.getMaterial()->emitted(rec.getU(), rec.getV(), rec.getHitPoint());
    if (!rec.getMaterial()->scatter(r, rec, attenuation, scattered))
    {
        return emitted;
    }

    return emitted + attenuation * ray_tracing_shader(scattered, world, background, depth - 1);
}


Color scattering_shader(const Ray &r, HittableList &world, Background &background, int depth)
{
    HitRecord rec = HitRecord();
    if (world.hit(r, 0.001f, 100.0f, rec))
    {
        Ray scattered;
        Color attenuation;

        if (rec.getMaterial()->scatter(r, rec, attenuation, scattered)) 
        {
            return scattered.direction*0.5 + Vec3(0.5,0.5, 0.5);
        }
    }
    
    return background.get_color(r);
}


Color normal_shader(const Ray &r, HittableList &world, Background &background, int depth)
{
    HitRecord rec;
    if (!world.hit(r, 0.001f, 100.0f, rec))
    {
        return background.get_color(r);
    }
    
    Vec3 normal = rec.isFrontFace(r) ? rec.getNormal() : -rec.getNormal();
    return normal*0.5 + Vec3(0.5, 0.5, 0.5);
}
