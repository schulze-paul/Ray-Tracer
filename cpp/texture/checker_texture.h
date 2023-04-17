
class CheckerTexture : public Texture
{
public:
    CheckerTexture() {}
    CheckerTexture(Texture *t0, Texture *t1) : even(t0), odd(t1) {}

    virtual Color value(double u, double v, const Vec3 &p) const override
    {
        auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        if (sines < 0)
        {
            return odd->value(u, v, p);
        }
        else
        {
            return even->value(u, v, p);
        }
    }

private:
    Texture *even;
    Texture *odd;
};