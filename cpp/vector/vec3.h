#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

#include "ray_tracer.h"

class Vec3
{
public:
    double e[3];

    Vec3() : e{0, 0, 0} {}
    Vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}
    inline const Vec3 &operator+() const { return *this; }
    inline Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
    inline double operator[](int i) const { return e[i]; }
    inline double &operator[](int i) { return e[i]; }

    inline Vec3 &operator+=(const Vec3 &v2);
    inline Vec3 &operator-=(const Vec3 &v2);
    inline Vec3 &operator*=(const Vec3 &v2);
    inline Vec3 &operator/=(const Vec3 &v2);
    inline Vec3 &operator*=(const double t);
    inline Vec3 &operator/=(const double t);

    inline static Vec3 random(double min, double max);

    inline double x() const
    {
        return e[0];
    }
    inline double y() const
    {
        return e[1];
    }
    inline double z() const
    {
        return e[2];
    }

    inline double length() const
    {
        return std::sqrt(length_squared());
    }
    inline double length_squared() const
    {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }
};

// for printing
inline std::ostream &operator<<(std::ostream &out, const Vec3 &v)
{
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

// addition
inline Vec3 operator+(const Vec3 &v1, const Vec3 &v2)
{
    return Vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

// subtraction
inline Vec3 operator-(const Vec3 &v1, const Vec3 &v2)
{
    return Vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

// multiplication
inline Vec3 operator*(const Vec3 &v1, const Vec3 &v2)
{
    return Vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

// division
inline Vec3 operator/(const Vec3 &v1, const Vec3 &v2)
{
    return Vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

// multiplication by scalar
inline Vec3 operator*(double t, const Vec3 &v)
{
    return Vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}
// multiplication by scalar
inline Vec3 operator*(const Vec3 &v, double t)
{
    return t * v;
}

// division by scalar
inline Vec3 operator/(Vec3 v, double t)
{
    return v * (1 / t);
}

// dot product
inline double dot(const Vec3 &v1, const Vec3 &v2)
{
    return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

// cross product
inline Vec3 cross(const Vec3 &v1, const Vec3 &v2)
{
    return Vec3(v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1],
                v1.e[2] * v2.e[0] - v1.e[0] * v2.e[2],
                v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]);
}

// unit vector
inline Vec3 unit_vector(Vec3 v)
{
    return v / v.length();
}

// Vec3 member functions
inline Vec3 &Vec3::operator+=(const Vec3 &v)
{
    e[0] += v.e[0];
    e[1] += v.e[1];
    e[2] += v.e[2];
    return *this;
}

inline Vec3 &Vec3::operator*=(const Vec3 &v)
{
    e[0] *= v.e[0];
    e[1] *= v.e[1];
    e[2] *= v.e[2];
    return *this;
}

inline Vec3 &Vec3::operator/=(const Vec3 &v)
{
    e[0] /= v.e[0];
    e[1] /= v.e[1];
    e[2] /= v.e[2];
    return *this;
}

inline Vec3 &Vec3::operator-=(const Vec3 &v)
{
    e[0] -= v.e[0];
    e[1] -= v.e[1];
    e[2] -= v.e[2];
    return *this;
}

inline Vec3 &Vec3::operator*=(const double t)
{
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
}

inline Vec3 &Vec3::operator/=(const double t)
{
    e[0] /= t;
    e[1] /= t;
    e[2] /= t;
    return *this;
}

inline Vec3 Vec3::random(double min, double max)
{
    return Vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

inline Vec3 random_in_unit_sphere()
{
    while (true)
    {
        Vec3 p = Vec3::random(-1, 1);
        if (p.length_squared() >= 1)
            continue;
        return p;
    }
}

inline Vec3 random_unit_vector()
{
    return unit_vector(random_in_unit_sphere());
}

inline Vec3 random_in_hemisphere(const Vec3 &normal)
{
    Vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0)
    {
        return in_unit_sphere;
    }
    else
    {
        return -in_unit_sphere;
    }
}

inline Vec3 random_in_unit_disk()
{
    while (true)
    {
        Vec3 p = Vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() >= 1)
            continue;
        return p;
    }
}

#endif