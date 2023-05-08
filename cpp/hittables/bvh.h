/*
BVHNode is a binary tree structure that is used to speed up ray tracing.
It is used to speed up ray-object intersection tests.

The tree is constructed by recursively splitting the objects into two groups
until each group contains only one object. The splitting is done by choosing
the longest axis of the bounding box of the objects and splitting the objects
into two groups based on the median of the longest axis.

The tree is traversed by checking if the ray hits the bounding box of the node.
If the ray hits the bounding box, then the ray is recursively checked against
the left and right child nodes. If the ray does not hit the bounding box, then
the ray does not hit any of the objects in the node.
*/

#ifndef BVG_H
#define BVG_H

#include <vector>
#include <memory>
#include <algorithm>

#include "hittable.h"
#include "aabb.h"

/**
 * @brief      Class for bounding volume hierarchy node.
 */
class BVHNode : public Hittable
{
public:
    BVHNode() {}
    BVHNode(std::vector<std::shared_ptr<Hittable>> &objects, size_t start, size_t end, double time0, double time1);

    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const override;
    virtual bool bounding_box(double time0, double time1, AABB &output_box) const override;
    std::string to_string() const { return "BVHNode"; }
public:
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;
    AABB box;
};

bool box_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis);
bool box_x_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);
bool box_y_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);
bool box_z_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);

#endif // BVG_H