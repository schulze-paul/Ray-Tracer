#ifndef IMAGE_DATA_H
#define IMAGE_DATA_H

#include <vector>
#include <iostream>
#include <fstream>
#include "color.h"
#include "ray_tracer.h"

double gamma_correction(double x)
{
    return std::pow(x, 1 / 2.2);
}

class ImageData
{
private:
    int width;
    int height;
    double aspect_ratio;
    std::vector<std::vector<std::vector<double>>> pixels;
    std::vector<std::vector<int>> number_of_samples;

public:
    ImageData(){};
    ImageData(int width, double aspect_ratio);
    int get_width() const;
    int get_height() const;
    int write_ppm(std::ostream &out);
    void write_to_file(std::string filename);
    int add_color(int i, int j, Color color);
    double get_aspect_ratio() const;
    double get_u(int i) const;
    double get_v(int j) const;
};
ImageData::ImageData(int width, double aspect_ratio)
{
    this->width = width;
    this->height = width / aspect_ratio;
    this->aspect_ratio = aspect_ratio;
    this->pixels = std::vector<std::vector<std::vector<double>>>(this->height, std::vector<std::vector<double>>(this->width, std::vector<double>(3, 0.0f)));
    this->number_of_samples = std::vector<std::vector<int>>(this->height, std::vector<int>(this->width, 0));
}
int ImageData::add_color(int i, int j, Color color)
{
    this->pixels[j][i][0] += color.r();
    this->pixels[j][i][1] += color.g();
    this->pixels[j][i][2] += color.b();
    this->number_of_samples[j][i] += 1;
    return 0;
}

int ImageData::get_width() const
{
    return this->width;
}

int ImageData::get_height() const
{
    return this->height;
}

double ImageData::get_aspect_ratio() const
{
    return this->aspect_ratio;
}

int ImageData::write_ppm(std::ostream &out)
{

    // write header
    out << "P3\n"
        << this->width << " " << this->height << "\n255\n";

    // write pixels
    int r;
    int g;
    int b;
    int number_of_samples;
    for (int j = this->height - 1; j >= 0; --j)
    {
        for (int i = 0; i < this->width; ++i)
        {
            // normalize color and convert to int
            number_of_samples = this->number_of_samples[j][i];
            r = int(255.999 * gamma_correction(this->pixels[j][i][0] / number_of_samples));
            g = int(255.999 * gamma_correction(this->pixels[j][i][1] / number_of_samples));
            b = int(255.999 * gamma_correction(this->pixels[j][i][2] / number_of_samples));
            r = clamp(r, 0, 255);
            g = clamp(g, 0, 255);
            b = clamp(b, 0, 255);

            out << r << " " << g << " " << b << "\n";
        }
    }
    return 0;
}

void ImageData::write_to_file(std::string filename)
{
    std::cout << "Writing to file " << filename << std::endl;
    std::ofstream out(filename); // opens the file
    this->write_ppm(out);       // write to the file
    out.close();            // close the file

}   

double ImageData::get_u(int i) const
{
    double pixel_center = (i + 0.5) / this->width;
    double random_offset = (random_double() - 0.5) / this->width;
    return pixel_center + random_offset;
}

double ImageData::get_v(int j) const
{
    double pixel_center = (j + 0.5) / this->height;
    double random_offset = (random_double() - 0.5) / this->height;
    return pixel_center + random_offset;
}

#endif // IMAGE_DATA_H