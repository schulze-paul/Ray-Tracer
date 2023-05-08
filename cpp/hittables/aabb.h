/*
Axis-aligned bounding box
=========================
*/

#ifndef AABB_H
#define AABB_H

/**
 * @brief      Class for axis-aligned bounding box.
 * @details    An AABB is a box that is aligned with the x, y, and z axes that contains an object.
 *             It is used to speed up ray-object intersection tests.
 */
class AABB
{
public:
    AABB() {}
    AABB(const Vec3 &a, const Vec3 &b)
    {
        minimum = a;
        maximum = b;
    }

    Vec3 get_min() const { return minimum; }
    Vec3 get_max() const { return maximum; }

    bool hit(const Ray &r, double t_min, double t_max) const;

    Vec3 minimum;
    Vec3 maximum;
};

/**
 * @brief      Determines if the ray hits the AABB.
 * @param[in]  r       The ray
 * @param[in]  t_min   The minimum distance along the ray
 * @param[in]  t_max   The maximum distance along the ray
 * @return     True if the ray hits the AABB, False otherwise.
 */
inline bool AABB::hit(const Ray &r, double t_min, double t_max) const
{
    for (int a = 0; a < 3; a++)
    {
        double invD = 1.0 / r.direction[a];
        double t0 = (minimum[a] - r.origin[a]) * invD;
        double t1 = (maximum[a] - r.origin[a]) * invD;
        if (invD < 0.0)
        {
            std::swap(t0, t1);
        }
        t_min = t0 > t_min ? t0 : t_min;
        t_max = t1 < t_max ? t1 : t_max;
        if (t_max <= t_min)
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief      Constructs an AABB that contains two AABBs.
 * @param[in]  box0  The first AABB
 * @param[in]  box1  The second AABB
 * @return     An AABB that contains both box0 and box1.
 */
AABB surrounding_box(AABB box0, AABB box1)
{
    Vec3 small(fmin(box0.minimum.x(), box1.minimum.x()),
               fmin(box0.minimum.y(), box1.minimum.y()),
               fmin(box0.minimum.z(), box1.minimum.z()));
    Vec3 big(fmax(box0.maximum.x(), box1.maximum.x()),
             fmax(box0.maximum.y(), box1.maximum.y()),
             fmax(box0.maximum.z(), box1.maximum.z()));
    return AABB(small, big);
}

#endif