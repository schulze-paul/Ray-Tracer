
#include "texture.h"

class NoiseTexture : public Texture
{
public:
    NoiseTexture() {}
    NoiseTexture(double sc) : scale(sc) {}

    virtual Color value(double u, double v, const Vec3 &p) const override
    {
        return Color(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turb(scale * p)));
    }

private:
    Perlin noise;
    double scale;
};
