#include <memory>
#include <vector>
#include <cmath>

#include "color.h"
#include "ray.h"
#include "hit_record.h"
#include "hittable_list.h"
#include "background.h"
#include "pdf.h"
#include "spectra.h"

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

IntensitySpectrum white_point = get_black_body_spectrum(WHITE_POINT_TEMPERATURE);
double ray_tracing_shader_wavelength(const Ray &ray_in, HittableList &world, Background &background, int depth);

IntensitySpectrum spectral_ray_tracing_shader(Ray &ray_in, HittableList &world, Background &background, int depth)
{
    IntensitySpectrum spectrum = IntensitySpectrum();
    int number_iterations = spectrum.size();
    double intensity;
    int wavelength_index;
    // sample wavelengths for ray
    for (int index = 0; index < number_iterations; index++) {
        wavelength_index = random_int(0, spectrum.size() - 1);
        ray_in.wavelength = white_point.get_wavelength(wavelength_index);

        intensity = white_point[wavelength_index]*ray_tracing_shader_wavelength(ray_in, world, background, depth);
        // check for NaN
        if (!(intensity != intensity))
            spectrum[wavelength_index] += intensity;
    }
    return spectrum;
}

double ray_tracing_shader_wavelength(const Ray &ray_in, HittableList &world, Background &background, int depth)
{
    
    if (depth <= 0) {
        // max depth reached
        return 0;
    }

    HitRecord hit_rec;
    if (!world.hit(ray_in, 0.001f, infinity, hit_rec)) {
        // background
        return background.get_color(ray_in).sum()/3;
        }

    auto white_spectrum = get_black_body_spectrum(WHITE_POINT_TEMPERATURE);

    ScatterRecord scatter_record;
    double emitted = hit_rec.get_material()->emitted(hit_rec.u, hit_rec.v, hit_rec.get_hit_point()).sum()/3;
    if (!hit_rec.get_material()->scatter(ray_in, hit_rec, scatter_record)) {
        // no scatter
        return emitted;
    }

    if (scatter_record.is_specular) {
        // specular reflection or refraction
        return scatter_record.attenuation.sum()/3 * ray_tracing_shader_wavelength(scatter_record.specular_ray, world, background, depth - 1);
    }

    // diffuse
    Vec3 direction;
    double pdf_val;
    scatter_record.pdf->generate(direction, pdf_val);
    Ray scattered = Ray(hit_rec.get_hit_point(), direction, ray_in.get_time(), ray_in.wavelength);
    return emitted + scatter_record.attenuation.sum()/3 * hit_rec.get_material()->scattering_pdf(ray_in, hit_rec, scattered) * ray_tracing_shader_wavelength(scattered, world, background, depth - 1) / pdf_val;

}


Color normal_shader(const Ray &r, HittableList &world, Background &background, int depth)
{
    HitRecord rec;
    if (!world.hit(r, 0.001f, 100.0f, rec))
    {
        return background.get_color(r);
    }
    
    Vec3 normal = rec.is_front_face(r) ? rec.get_normal() : -rec.get_normal();
    return normal*0.5 + Vec3(0.5, 0.5, 0.5);
}

Color distance_shader(const Ray &r, HittableList &world, Background &background, int depth)
{
    HitRecord rec;
    if (!world.hit(r, 0.001f, 100.0f, rec))
    {
        return background.get_color(r);
    }
    return Vec3(rec.get_t(), rec.get_t(), rec.get_t());
}