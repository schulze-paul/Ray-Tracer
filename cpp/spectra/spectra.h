#ifndef SPECTRA_H
#define SPECTRA_H

/*
                Colour Rendering of Spectra

                       by John Walker
                  http://www.fourmilab.ch/

                 Last updated: March 9, 2003

           This program is in the public domain.

    For complete information about the techniques employed in
    this program, see the World-Wide Web document:

             http://www.fourmilab.ch/documents/specrend/

    The xyz_to_rgb() function, which was wrong in the original
    version of this program, was corrected by:

            Andrew J. S. Hamilton 21 May 1999
            Andrew.Hamilton@Colorado.EDU
            http://casa.colorado.edu/~ajsh/

    who also added the gamma correction facilities and
    modified constrain_rgb() to work by desaturating the
    colour by adding white.

    A program which uses these functions to plot CIE
    "tongue" diagrams called "ppmcie" is included in
    the Netpbm graphics toolkit:
        http://netpbm.sourceforge.net/
    (The program was called cietoppm in earlier
    versions of Netpbm.)

*/

#include <iostream>
#include <cmath>
#include <string>

#include "color.h"
#include "vec3.h"

const int MIN_WAVELENGTH = 380; // nm
const int MAX_WAVELENGTH = 780; // nm
const int DELTA_LAMBDA = 5;     // nm
const int NSPECTRUM = (780 - 380) / DELTA_LAMBDA + 1; // 81

const int WHITE_POINT_TEMPERATURE = 5563; // K

static double cie_colour_match[81][3] = {
        {0.0014,0.0000,0.0065}, {0.0022,0.0001,0.0105}, {0.0042,0.0001,0.0201},
        {0.0076,0.0002,0.0362}, {0.0143,0.0004,0.0679}, {0.0232,0.0006,0.1102},
        {0.0435,0.0012,0.2074}, {0.0776,0.0022,0.3713}, {0.1344,0.0040,0.6456},
        {0.2148,0.0073,1.0391}, {0.2839,0.0116,1.3856}, {0.3285,0.0168,1.6230},
        {0.3483,0.0230,1.7471}, {0.3481,0.0298,1.7826}, {0.3362,0.0380,1.7721},
        {0.3187,0.0480,1.7441}, {0.2908,0.0600,1.6692}, {0.2511,0.0739,1.5281},
        {0.1954,0.0910,1.2876}, {0.1421,0.1126,1.0419}, {0.0956,0.1390,0.8130},
        {0.0580,0.1693,0.6162}, {0.0320,0.2080,0.4652}, {0.0147,0.2586,0.3533},
        {0.0049,0.3230,0.2720}, {0.0024,0.4073,0.2123}, {0.0093,0.5030,0.1582},
        {0.0291,0.6082,0.1117}, {0.0633,0.7100,0.0782}, {0.1096,0.7932,0.0573},
        {0.1655,0.8620,0.0422}, {0.2257,0.9149,0.0298}, {0.2904,0.9540,0.0203},
        {0.3597,0.9803,0.0134}, {0.4334,0.9950,0.0087}, {0.5121,1.0000,0.0057},
        {0.5945,0.9950,0.0039}, {0.6784,0.9786,0.0027}, {0.7621,0.9520,0.0021},
        {0.8425,0.9154,0.0018}, {0.9163,0.8700,0.0017}, {0.9786,0.8163,0.0014},
        {1.0263,0.7570,0.0011}, {1.0567,0.6949,0.0010}, {1.0622,0.6310,0.0008},
        {1.0456,0.5668,0.0006}, {1.0026,0.5030,0.0003}, {0.9384,0.4412,0.0002},
        {0.8544,0.3810,0.0002}, {0.7514,0.3210,0.0001}, {0.6424,0.2650,0.0000},
        {0.5419,0.2170,0.0000}, {0.4479,0.1750,0.0000}, {0.3608,0.1382,0.0000},
        {0.2835,0.1070,0.0000}, {0.2187,0.0816,0.0000}, {0.1649,0.0610,0.0000},
        {0.1212,0.0446,0.0000}, {0.0874,0.0320,0.0000}, {0.0636,0.0232,0.0000},
        {0.0468,0.0170,0.0000}, {0.0329,0.0119,0.0000}, {0.0227,0.0082,0.0000},
        {0.0158,0.0057,0.0000}, {0.0114,0.0041,0.0000}, {0.0081,0.0029,0.0000},
        {0.0058,0.0021,0.0000}, {0.0041,0.0015,0.0000}, {0.0029,0.0010,0.0000},
        {0.0020,0.0007,0.0000}, {0.0014,0.0005,0.0000}, {0.0010,0.0004,0.0000},
        {0.0007,0.0002,0.0000}, {0.0005,0.0002,0.0000}, {0.0003,0.0001,0.0000},
        {0.0002,0.0001,0.0000}, {0.0002,0.0001,0.0000}, {0.0001,0.0000,0.0000},
        {0.0001,0.0000,0.0000}, {0.0001,0.0000,0.0000}, {0.0000,0.0000,0.0000}
    };





/* A colour system is defined by the CIE x and y coordinates of
   its three primary illuminants and the x and y coordinates of
   the white point. */

struct colour_system {
    std::string name;                 /* Colour system name */
    double x_red, y_red,              /* Red x, y */
           x_green, y_green,          /* Green x, y */
           x_blue, y_blue,            /* Blue x, y */
           x_white, y_white,          /* White point x, y */
           gamma;                   /* Gamma correction for system */
};

/* White point chromaticities. */

#define illuminant_c     0.3101, 0.3162          /* For NTSC television */
#define illuminant_d65   0.3127, 0.3291          /* For EBU and SMPTE */
#define illuminant_e     0.33333333, 0.33333333  /* CIE equal-energy illuminant */

/*  Gamma of nonlinear correction.

    See Charles Poynton's ColorFAQ Item 45 and GammaFAQ Item 6 at:

       http://www.poynton.com/ColorFAQ.html
       http://www.poynton.com/GammaFAQ.html

*/

#define GAMMA_REC709    0               /* Rec. 709 */

static struct colour_system
                  /* Name                  xRed    yRed    xGreen  yGreen  xBlue  yBlue    White point        Gamma   */
    NTSC_system  =  { "NTSC",               0.67,   0.33,   0.21,   0.71,   0.14,   0.08,   illuminant_c,    GAMMA_REC709 },
    EBU_system   =  { "EBU (PAL/SECAM)",    0.64,   0.33,   0.29,   0.60,   0.15,   0.06,   illuminant_d65,  GAMMA_REC709 },
    SMPTE_system =  { "SMPTE",              0.630,  0.340,  0.310,  0.595,  0.155,  0.070,  illuminant_d65,  GAMMA_REC709 },
    HDTV_system  =  { "HDTV",               0.670,  0.330,  0.210,  0.710,  0.150,  0.060,  illuminant_d65,  GAMMA_REC709 },
    CIE_system   =  { "CIE",                0.7355, 0.2645, 0.2658, 0.7243, 0.1669, 0.0085, illuminant_e,    GAMMA_REC709 },
    Rec709_system = { "CIE REC 709",        0.64,   0.33,   0.30,   0.60,   0.15,   0.06,   illuminant_d65,  GAMMA_REC709 };

// CIE colour matching functions xBar, yBar, and zBar for
//   wavelengths from 380 through 780 nanometers, every 5
//   nanometers.  For a wavelength lambda in this range:
//        cie_colour_match[(lambda - 380) / 5][0] = xBar
//        cie_colour_match[(lambda - 380) / 5][1] = yBar
//        cie_colour_match[(lambda - 380) / 5][2] = zBar

class IntensitySpectrum;
IntensitySpectrum get_black_body_spectrum(double temperature);

/*                          UPVP_TO_XY

    Given 1976 coordinates u', v', determine 1931 chromaticities x, y

*/


void upvp_to_xy(double up, double vp, double &xc, double &yc)
{
    xc = (9 * up) / ((6 * up) - (16 * vp) + 12);
    yc = (4 * vp) / ((6 * up) - (16 * vp) + 12);
}

/*                          XY_TO_UPVP

    Given 1931 chromaticities x, y, determine 1976 coordinates u', v'

*/

void xy_to_upvp(double xc, double yc, double &up, double &vp)
{
    up = (4 * xc) / ((-2 * xc) + (12 * yc) + 3);
    vp = (9 * yc) / ((-2 * xc) + (12 * yc) + 3);
}

/*                             XYZ_TO_RGB

    Given an additive tricolour system CS, defined by the CIE x
    and y chromaticities of its three primaries (z is derived
    trivially as 1-(x+y)), and a desired chromaticity (XC, YC,
    ZC) in CIE space, determine the contribution of each
    primary in a linear combination which sums to the desired
    chromaticity.  If the  requested chromaticity falls outside
    the Maxwell  triangle (colour gamut) formed by the three
    primaries, one of the r, g, or b weights will be negative.

    Caller can use constrain_rgb() to desaturate an
    outside-gamut colour to the closest representation within
    the available gamut and/or norm_rgb to normalise the RGB
    components so the largest nonzero component has value 1.

*/

void xyz_to_rgb(struct colour_system &cs,
                const Vec3 &xyz_c,
                Color &rgb_c)
{
    double xc = xyz_c.x(), yc = xyz_c.y(), zc = xyz_c.z();

    double xr, yr, zr, xg, yg, zg, xb, yb, zb;
    double xw, yw, zw;
    double rx, ry, rz, gx, gy, gz, bx, by, bz;
    double rw, gw, bw;

    xr = cs.x_red;    yr = cs.y_red;    zr = 1 - (xr + yr);
    xg = cs.x_green;  yg = cs.y_green;  zg = 1 - (xg + yg);
    xb = cs.x_blue;   yb = cs.y_blue;   zb = 1 - (xb + yb);

    xw = cs.x_white;  yw = cs.y_white;  zw = 1 - (xw + yw);

    /* xyz -> rgb matrix, before scaling to white. */

    rx = (yg * zb) - (yb * zg);  ry = (xb * zg) - (xg * zb);  rz = (xg * yb) - (xb * yg);
    gx = (yb * zr) - (yr * zb);  gy = (xr * zb) - (xb * zr);  gz = (xb * yr) - (xr * yb);
    bx = (yr * zg) - (yg * zr);  by = (xg * zr) - (xr * zg);  bz = (xr * yg) - (xg * yr);

    /* White scaling factors.
       Dividing by yw scales the white luminance to unity, as conventional. */

    rw = ((rx * xw) + (ry * yw) + (rz * zw)) / yw;
    gw = ((gx * xw) + (gy * yw) + (gz * zw)) / yw;
    bw = ((bx * xw) + (by * yw) + (bz * zw)) / yw;

    /* xyz -> rgb matrix, correctly scaled to white. */

    rx = rx / rw;  ry = ry / rw;  rz = rz / rw;
    gx = gx / gw;  gy = gy / gw;  gz = gz / gw;
    bx = bx / bw;  by = by / bw;  bz = bz / bw;

    /* rgb of the desired point */

    double r = (rx * xc) + (ry * yc) + (rz * zc);
    double g = (gx * xc) + (gy * yc) + (gz * zc);
    double b = (bx * xc) + (by * yc) + (bz * zc);
    rgb_c = Color(r, g, b);
}

/*                            INSIDE_GAMUT

     Test whether a requested colour is within the gamut
     achievable with the primaries of the current colour
     system.  This amounts simply to testing whether all the
     primary weights are non-negative. */

int inside_gamut(Color rgb)
{
    return (rgb.r() >= 0) && (rgb.g() >= 0) && (rgb.b() >= 0);
}

/*                          CONSTRAIN_RGB

    If the requested RGB shade contains a negative weight for
    one of the primaries, it lies outside the colour gamut
    accessible from the given triple of primaries.  Desaturate
    it by adding white, equal quantities of R, G, and B, enough
    to make RGB all positive.  The function returns 1 if the
    components were modified, zero otherwise.

*/

bool constrain_rgb(Color &rgb)
{
    double w;

    /* Amount of white needed is w = - min(0, r, g, b) */

    w = (0 < rgb.r()) ? 0 : rgb.r();
    w = (w < rgb.g()) ? w : rgb.g();
    w = (w < rgb.b()) ? w : rgb.b();
    w = -w;

    /* Add just enough white to make r, g, b all positive. */

    if (w > 0) {
        rgb += Color(w, w, w);
        return true;                     /* Colour modified to fit RGB gamut */
    }

    return false;                         /* Colour within RGB gamut */
}

/*                          GAMMA_CORRECT_RGB

    Transform linear RGB values to nonlinear RGB values. Rec.
    709 is ITU-R Recommendation BT. 709 (1990) ``Basic
    Parameter Values for the HDTV Standard for the Studio and
    for International Programme Exchange'', formerly CCIR Rec.
    709. For details see

       http://www.poynton.com/ColorFAQ.html
       http://www.poynton.com/GammaFAQ.html
*/

void gamma_correct(const struct colour_system &cs, double &c)
{
    double gamma;

    gamma = cs.gamma;

    if (gamma == GAMMA_REC709) {
        /* Rec. 709 gamma correction. */
        double cc = 0.018;

        if (c < cc) {
            c *= ((1.099 * pow(cc, 0.45)) - 0.099) / cc;
        } else {
            c = (1.099 * pow(c, 0.45)) - 0.099;
        }
    } else {
        /* Nonlinear colour = (Linear colour)^(1/gamma) */
        c = pow(c, 1.0 / gamma);
    }
}

void gamma_correct_rgb(const struct colour_system &cs, Color& rgb)
{
    gamma_correct(cs, rgb.e[0]);
    gamma_correct(cs, rgb.e[1]);
    gamma_correct(cs, rgb.e[2]);
}

/*                          NORM_RGB

    Normalise RGB components so the most intense (unless all
    are zero) has a value of 1.

*/

void norm_rgb(Color &rgb)
{
#define max(a, b)   (((a) > (b)) ? (a) : (b))
    double greatest = max(rgb.r(), max(rgb.g(), rgb.b()));

    if (greatest > 0) {
        rgb /= greatest;
    }
#undef max
}


/*                            BB_SPECTRUM

    Calculate, by Planck's radiation law, the emittance of a black body
    of temperature bbTemp at the given wavelength (in metres).  */

double bbTemp = 5000;                 /* Hidden temperature argument
                                         to BB_SPECTRUM. */
double bb_spectrum(double wavelength)
{
    double wlm = wavelength * 1e-9;   /* Wavelength in meters */

    return (3.74183e-16 * pow(wlm, -5.0)) /
           (exp(1.4388e-2 / (wlm * bbTemp)) - 1.0);
}

double get_white_spectrum_intensity(double wavelength)
{
    // 6500K black body spectrum
    bbTemp = 5560;
    return bb_spectrum(wavelength);
}

double target_wavelength = 380;
double get_delta_spectrum_intensity(double wavelength)
{
    if ((wavelength > target_wavelength - 0.1) and (wavelength < target_wavelength + 0.1))
        return get_white_spectrum_intensity(wavelength);
    else
        return 0;
}


class IntensitySpectrum 
{
public:
    double wavelengths[NSPECTRUM];
    double intensities[NSPECTRUM];

    IntensitySpectrum() {
        for (int i = 0; i < size(); i++) {
            wavelengths[i] = MIN_WAVELENGTH + DELTA_LAMBDA * i;
            intensities[i] = 0;
        }
    }

    IntensitySpectrum(double intensities[NSPECTRUM]) {
        for (int i = 0; i < size(); i++) {
            wavelengths[i] = MIN_WAVELENGTH + DELTA_LAMBDA * i;
            this->intensities[i] = intensities[i];
        }
    }
    IntensitySpectrum(std::vector<double> intensities) {
        for (int i = 0; i < size(); i++) {
            wavelengths[i] = MIN_WAVELENGTH + DELTA_LAMBDA * i;
            this->intensities[i] = intensities[i];
        }
    }

    IntensitySpectrum(const IntensitySpectrum& other) {
        for (int i = 0; i < size(); i++) {
            wavelengths[i] = other.wavelengths[i];
            intensities[i] = other.intensities[i];
        }
    }

    inline double operator[](int i) const { return intensities[i]; }
    inline double &operator[](int i) { return intensities[i]; }
    inline void operator+=(const IntensitySpectrum& other) {
        for (int i = 0; i < size(); i++) {
            intensities[i] += other.intensities[i];
        }
    }
    
    inline int size() const {
        return NSPECTRUM;
    }

    inline int get_resolution() const {
        return DELTA_LAMBDA;
    }

    inline int get_min_wavelength() const {
        return wavelengths[0];
    }
    inline int get_max_wavelength() const {
        return wavelengths[size() - 1];
    }

    inline double get_wavelength(int i) const {
        return wavelengths[i];
    }

    inline double get_intensity(int i) const {
        return intensities[i];
    }

    Vec3 to_xyz() const {
        double x = 0, y = 0, z = 0;
        for (int i = 0; i < size(); i++) {
            double lambda = wavelengths[i];
            double intensity = intensities[i];
            x += cie_colour_match[i][0] * intensity;
            y += cie_colour_match[i][1] * intensity;
            z += cie_colour_match[i][2] * intensity;
        }
        Vec3 xyz(x, y, z);
        return xyz;
        return xyz/xyz.sum();
    }

    Color to_rgb() const {
        Vec3 xyz = to_xyz();
        Color rgb;
        xyz_to_rgb(CIE_system, xyz, rgb);
        // constrain_rgb(rgb);
        // norm_rgb(rgb);
        rgb *= get_power(); // normalize to white point black body spectrum
        return rgb;
    }

    void print() const {
        for (int i = 0; i < size(); i++) {
            printf("%f %f\n", wavelengths[i], intensities[i]);
        }
    }

    double get_power() const {
        double power = get_raw_power();
        double power_white_spectrum = get_black_body_spectrum(WHITE_POINT_TEMPERATURE).get_raw_power();
        return power/power_white_spectrum;
    }

    int sample_index() const {
        double total_intensity = get_raw_power();
        double random = random_double() * total_intensity;
        double sum = 0;
        for (int i = 0; i < size(); i++) {
            sum += intensities[i];
            if (sum > random) {
                return i;
            }
        }
        return size() - 1;
    }

    int sample_wavelength() const {
        int index = sample_index();
        return wavelengths[index];
    }

    double sample_intensity() const {
        int index = sample_index();
        return intensities[index];
    }

    void add_wavelength(double wavelength, double intensity) {
        int index = (wavelength - MIN_WAVELENGTH) / DELTA_LAMBDA;
        if (index < 0 or index >= size()) {
            return;
        }
        intensities[index] += intensity;
    }

    bool is_nan() const {
        for (int i = 0; i < size(); i++) {
            if (std::isnan(intensities[i])) {
                return true;
            }
        }
        return false;
    }

private:
    double get_raw_power() const {
        double power = 0;
        for (int i = 0; i < size(); i++) {
            power += intensities[i];
        }
        return power;
    }
};

IntensitySpectrum get_black_body_spectrum(double temperature)
{
    IntensitySpectrum result;
    bbTemp = temperature;
    for (int i = 0; i < result.size(); i++) {
        double lambda = result.wavelengths[i];
        double intensity = bb_spectrum(lambda);
        result.intensities[i] = intensity;
    }
    return result;
}

IntensitySpectrum operator*(IntensitySpectrum spectrum, double scalar)  {
    IntensitySpectrum result;
    for (int i = 0; i < result.size(); i++) {
        result.intensities[i] = spectrum.intensities[i] * scalar;
    }
    return result;
}

IntensitySpectrum operator*(double scalar, IntensitySpectrum spectrum)  {
    return spectrum * scalar;
}

IntensitySpectrum operator/(IntensitySpectrum spectrum, double scalar)  {
    IntensitySpectrum result;
    for (int i = 0; i < result.size(); i++) {
        result.intensities[i] = spectrum.intensities[i] / scalar;
    }
    return result;
}

IntensitySpectrum operator+(IntensitySpectrum spectrum1, IntensitySpectrum spectrum2)  {
    IntensitySpectrum result;
    for (int i = 0; i < result.size(); i++) {
        result.intensities[i] = spectrum1.intensities[i] + spectrum2.intensities[i];
    }
    return result;
}

IntensitySpectrum operator-(IntensitySpectrum spectrum1, IntensitySpectrum spectrum2)  {
    IntensitySpectrum result;
    for (int i = 0; i < result.size(); i++) {
        result.intensities[i] = spectrum1.intensities[i] - spectrum2.intensities[i];
    }
    return result;
}

IntensitySpectrum operator*(IntensitySpectrum spectrum1, IntensitySpectrum spectrum2)  {
    IntensitySpectrum result;
    for (int i = 0; i < result.size(); i++) {
        result.intensities[i] = spectrum1.intensities[i] * spectrum2.intensities[i];
    }
    return result;
}

IntensitySpectrum operator/(IntensitySpectrum spectrum1, IntensitySpectrum spectrum2)  {
    IntensitySpectrum result;
    for (int i = 0; i < result.size(); i++) {
        result.intensities[i] = spectrum1.intensities[i] / spectrum2.intensities[i];
    }
    return result;
}







#endif // SPECTRUM_H