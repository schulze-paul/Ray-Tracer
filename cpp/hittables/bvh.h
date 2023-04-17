#ifndef BVG_H
#define BVG_H

#include <vector>
#include <memory>
#include <algorithm>

#include "hittable.h"
#include "aabb.h"

class BVHNode : public Hittable
{
public:
    BVHNode() {}
    BVHNode(const std::vector<std::shared_ptr<Hittable>> &objects, size_t start, size_t end, double time0, double time1);
    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const;
    virtual bool bounding_box(double t0, double t1, AABB &box) const;

private:
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;
    AABB box;
};

bool BVHNode::bounding_box(double t0, double t1, AABB &box) const
{
    box = this->box;
    return true;
}

bool BVHNode::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const
{
    if (!box.hit(r, t_min, t_max))
        return false;

    bool hit_left = left->hit(r, t_min, t_max, rec);
    bool hit_right = right->hit(r, t_min, hit_left ? rec.getT() : t_max, rec);

    return hit_left || hit_right;
}

BVHNode::BVHNode(const std::vector<std::shared_ptr<Hittable>> &objects, size_t start, size_t end, double time0, double time1)
{
    int axis = random_int(0, 2);
    auto comparator = (axis == 0)   ? box_x_compare
                      : (axis == 1) ? box_y_compare
                                    : box_z_compare;

    size_t object_span = end - start;

    if (object_span == 1)
    {
        left = right = objects[start];
    }
    else if (object_span == 2)
    {
        if (comparator(objects[start], objects[start + 1]))
        {
            left = objects[start];
            right = objects[start + 1];
        }
        else
        {
            left = objects[start + 1];
            right = objects[start];
        }
    }
    else
    {
        std::sort(objects.begin() + start, objects.begin() + end, comparator);

        auto mid = start + object_span / 2;
        left = std::make_shared<BVHNode>(objects, start, mid, time0, time1);
        right = std::make_shared<BVHNode>(objects, mid, end, time0, time1);
    }

    AABB box_left, box_right;

    if (!left->bounding_box(time0, time1, box_left) || !right->bounding_box(time0, time1, box_right))
        std::cerr << "No bounding box in BVHNode constructor.\n";

    box = surrounding_box(box_left, box_right);
}

inline bool box_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b, int axis)
{
    AABB box_a;
    AABB box_b;

    if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
        std::cerr << "No bounding box in BVHNode constructor.\n";

    return box_a.getMin().e[axis] < box_b.getMax().e[axis];
}

bool box_x_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b)
{
    return box_compare(a, b, 0);
}

bool box_y_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b)
{
    return box_compare(a, b, 1);
}

bool box_z_compare(const shared_prt<Hittable> a, const shared_prt<Hittable> b)
{
    return box_compare(a, b, 2);
}

#endif // BVG_H