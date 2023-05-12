/*
image_data.h
============
Represents the image data.

Contains functions to write the image data to a file.
*/

#ifndef IMAGE_DATA_H
#define IMAGE_DATA_H

#include <vector>
#include <iostream>
#include <fstream>
#include "color.h"
#include "ray_tracer.h"
#include "spectra.h"

/**
 * @brief      Gamma correction with gamma 2.2.
 * @param[in]  x     The value to be gamma corrected
 * @return     The gamma corrected value
 */
double gamma_correction(double x)
{
    return std::pow(x, 1 / 2.2);
}

/**
 * @brief      Class for image data.
 */
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
    ImageData(int width, int height);
    int get_width() const;
    int get_height() const;
    int write_ppm(std::ostream &out);
    void write_to_ppm(std::string filename);
    int add_color(int i, int j, Color color);
    double get_aspect_ratio() const;
    double get_u(int i) const;
    double get_v(int j) const;
    void normalize();
    void gamma_correct(double gamma);
    void invert();
};

/**
 * @brief      Constructs vectors to hold pixel rgb data.
 * @param[in]  width   The width of the image in pixels
 * @param[in]  height  The height of the image in pixels
*/
ImageData::ImageData(int width, int height)
{
    this->width = width;
    this->height = height;
    this->aspect_ratio = (double)width / (double)height;
    this->pixels = std::vector<std::vector<std::vector<double>>>(this->height, std::vector<std::vector<double>>(this->width, std::vector<double>(3, 0.0f)));
    this->number_of_samples = std::vector<std::vector<int>>(this->height, std::vector<int>(this->width, 0));
}

/**
 * @brief      Constructs vectors to hold pixel rgb data.
 * @param[in]  width         The width of the image in pixels
 * @param[in]  aspect_ratio  The aspect ratio of the image
 */
ImageData::ImageData(int width, double aspect_ratio)
{
    this->width = width;
    this->height = width / aspect_ratio;
    this->aspect_ratio = aspect_ratio;
    this->pixels = std::vector<std::vector<std::vector<double>>>(this->height, std::vector<std::vector<double>>(this->width, std::vector<double>(3, 0.0f)));
    this->number_of_samples = std::vector<std::vector<int>>(this->height, std::vector<int>(this->width, 0));
}
/**
 * @brief      Adds a color to the pixel at (i, j).
 *             The color is added to the pixel and the number of samples is incremented.
 * @param[in]  i      The i coordinate of the pixel
 * @param[in]  j      The j coordinate of the pixel
 * @param[in]  color  The color to be added
*/
int ImageData::add_color(int i, int j, Color color)
{
    // remove NaNs
    if (color.x() != color.x()) color = Color(0,0,0);
    if (color.y() != color.y()) color = Color(0,0,0);
    if (color.z() != color.z()) color = Color(0,0,0);
                
    this->pixels[j][i][0] += color.r();
    this->pixels[j][i][1] += color.g();
    this->pixels[j][i][2] += color.b();
    this->number_of_samples[j][i] += 1;
    return 0;
}

/**
 * @brief      Get image width in pixels.
*/
int ImageData::get_width() const
{
    return this->width;
}

/**
 * @brief      Get image height in pixels.
*/
int ImageData::get_height() const
{
    return this->height;
}

/**
 * @brief      Get image aspect ratio.
*/
double ImageData::get_aspect_ratio() const
{
    return this->aspect_ratio;
}

/**
 * @brief      Write ppm image data to a file stream.
 *             The color of each pixel is averaged over the number of samples.
 *             The color is gamma corrected.
 * @param      out   The output stream
*/
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

/**
 * @brief      Write ppm image data to a file.
 *             The color of each pixel is averaged over the number of samples.
 *             The color is gamma corrected.
 * @param[in]  filename  The filename
*/
void ImageData::write_to_ppm(std::string filename)
{
    std::cout << "Writing to file " << filename << std::endl;
    std::ofstream out(filename); // opens the file
    this->write_ppm(out);       // write to the file
    out.close();            // close the file

}   

/**
 * @brief      Get u coordinate of pixel.
 *             The u coordinate is the horizontal coordinate of the pixel center.
 *             The u coordinate is randomly offset by a small amount to allow for antialiasing.
 * @param[in]  i     The i coordinate of the pixel
 * @return     The u coordinate of the pixel
*/
double ImageData::get_u(int i) const
{
    double pixel_center = (i + 0.5) / this->width;
    double random_offset = (random_double() - 0.5) / this->width;
    return pixel_center + random_offset;
}

/**
 * @brief      Get v coordinate of pixel.
 *             The v coordinate is the vertical coordinate of the pixel center.
 *             The v coordinate is randomly offset by a small amount to allow for antialiasing.
 * @param[in]  j     The j coordinate of the pixel
 * @return     The v coordinate of the pixel
*/
double ImageData::get_v(int j) const
{
    double pixel_center = (j + 0.5) / this->height;
    double random_offset = (random_double() - 0.5) / this->height;
    return pixel_center + random_offset;
}

void ImageData::normalize()
{
    // scale color values to [0, 1]
    // scale max value to 1
    // scale min value to 0
    double max_value = 0.0;
    double min_value = 1000.0;
    double value;

    // find max and min values
    for (int i = 0; i < this->width; i++)
    {
        for (int j = 0; j < this->height; ++j)
        {
            value = this->pixels[j][i][0];
            if (value > max_value)
            {
                max_value = value;
            }
            if (value < min_value)
            {
                min_value = value;
            }

            value = this->pixels[j][i][1];
            if (value > max_value)
            {
                max_value = value;
            }
            if (value < min_value)
            {
                min_value = value;
            }

            value = this->pixels[j][i][2];
            if (value > max_value)
            {
                max_value = value;
            }
            if (value < min_value)
            {
                min_value = value;
            }
        }
    }

    std::cout << "max value: " << max_value << std::endl;
    std::cout << "min value: " << min_value << std::endl;

    // scale values
    double scale = 1.0 / (max_value - min_value);
    for (int i = 0; i < this->width; i++)
    {
        for (int j = 0; j < this->height; ++j)
        {
            this->pixels[j][i][0] = (this->pixels[j][i][0] - min_value) * scale;
            this->pixels[j][i][1] = (this->pixels[j][i][1] - min_value) * scale;
            this->pixels[j][i][2] = (this->pixels[j][i][2] - min_value) * scale;
            this->number_of_samples[j][i] = 1;
        }
    }

}

void ImageData::gamma_correct(double gamma)
{
    int number_of_samples;
    double exponent = 1/gamma;
    for (int i = 0; i < this->width; i++)
    {
        for (int j = 0; j < this->height; ++j)
        {
            number_of_samples = this->number_of_samples[j][i];
            this->pixels[j][i][0] = pow(this->pixels[j][i][0]/number_of_samples, exponent);
            this->pixels[j][i][1] = pow(this->pixels[j][i][1]/number_of_samples, exponent);
            this->pixels[j][i][2] = pow(this->pixels[j][i][2]/number_of_samples, exponent);
            this->number_of_samples[j][i] = 1;
        }
    }
}

void ImageData::invert()
{
    for (int i = 0; i < this->width; i++)
    {
        for (int j = 0; j < this->height; ++j)
        {
            this->pixels[j][i][0] = 1.0 - this->pixels[j][i][0];
            this->pixels[j][i][1] = 1.0 - this->pixels[j][i][1];
            this->pixels[j][i][2] = 1.0 - this->pixels[j][i][2];
        }
    }
}



class SpectralImageData : public ImageData
{
private:
    std::vector<std::vector<std::vector<double>>> pixels;
    std::vector<std::vector<int>> number_of_samples;
    int width;
    int height;
    double aspect_ratio;

public:
    SpectralImageData(int width, int height);
    SpectralImageData(int width, double aspect_ratio);
    int add_spectrum(int i, int j, IntensitySpectrum spectrum);
    int get_width() const;
    int get_height() const;
    double get_aspect_ratio() const;
    int write_ppm(std::ostream &out);
    void write_to_ppm(std::string filename);
    double get_u(int i) const;
    double get_v(int j) const;
};

SpectralImageData::SpectralImageData(int width, int height)
{
    this->width = width;
    this->height = height;
    this->aspect_ratio = double(width) / height;
    this->pixels = std::vector<std::vector<std::vector<double>>>(this->height, std::vector<std::vector<double>>(this->width, std::vector<double>(NSPECTRUM, {0.0f})));
    this->number_of_samples = std::vector<std::vector<int>>(this->height, std::vector<int>(this->width, 0));
}

SpectralImageData::SpectralImageData(int width, double aspect_ratio)
{
    this->width = width;
    this->height = int(width / aspect_ratio);
    this->aspect_ratio = aspect_ratio;
    this->pixels = std::vector<std::vector<std::vector<double>>>(this->height, std::vector<std::vector<double>>(this->width, std::vector<double>(NSPECTRUM, {0.0f})));
    this->number_of_samples = std::vector<std::vector<int>>(this->height, std::vector<int>(this->width, 0));
}

int SpectralImageData::add_spectrum(int i, int j, IntensitySpectrum spectrum)
{
    for (int k = 0; k < NSPECTRUM; ++k)
        this->pixels[j][i][k] += spectrum[k];
    this->number_of_samples[j][i] += 1;
    return 0;
}

int SpectralImageData::get_width() const
{
    return this->width;
}

int SpectralImageData::get_height() const
{
    return this->height;
}

double SpectralImageData::get_aspect_ratio() const
{
    return this->aspect_ratio;
}

int SpectralImageData::write_ppm(std::ostream &out)
{

    // write header
    out << "P3\n"
        << this->width << " " << this->height << "\n255\n";

    // write pixels
    int r;
    int g;
    int b;
    Color rgb;
    IntensitySpectrum spectrum;
    int number_of_samples;
    for (int j = this->height - 1; j >= 0; --j)
    {
        for (int i = 0; i < this->width; ++i)
        {
            number_of_samples = this->number_of_samples[j][i];
            
            spectrum = IntensitySpectrum(this->pixels[j][i]);
            spectrum = 1/number_of_samples * spectrum;

            rgb = spectrum.to_rgb();
            r = int(255.999 * rgb.r());
            g = int(255.999 * rgb.g());
            b = int(255.999 * rgb.b());
            r = clamp(r, 0, 255);
            g = clamp(g, 0, 255);
            b = clamp(b, 0, 255);

            out << r << " " << g << " " << b << "\n";
        }
    }
    return 0;
}

void SpectralImageData::write_to_ppm(std::string filename)
{
    std::cout << "Writing to file " << filename << std::endl;
    std::ofstream out(filename); // opens the file
    this->write_ppm(out);       // write to the file
    out.close();            // close the file
}

double SpectralImageData::get_u(int i) const
{
    double pixel_center = (i + 0.5) / this->width;
    double random_offset = (random_double() - 0.5) / this->width;
    return pixel_center + random_offset;
}

double SpectralImageData::get_v(int j) const
{
    double pixel_center = (j + 0.5) / this->height;
    double random_offset = (random_double() - 0.5) / this->height;
    return pixel_center + random_offset;
}

#endif // IMAGE_DATA_H