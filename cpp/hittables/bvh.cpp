
#include "bvh.h"

/**
 * @brief      Constructs the BHV tree.
 * @param[in]  objects  The objects
 * @param[in]  start    The start index
 * @param[in]  end      The end index
 * @param[in]  time0    The time 0
 * @param[in]  time1    The time 1
 */
BVHNode::BVHNode(std::vector<std::shared_ptr<Hittable>> &objects, size_t start, size_t end, double time0, double time1)
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
    {
        std::cerr << "No bounding box in BVHNode constructor." << std::endl;
    }

    box = surrounding_box(box_left, box_right);
}

/**
 * @brief      Checks if the ray hits the bounding box of the node.
 * @param[in]  r       The ray
 * @param[in]  t_min   The minimum parameter value
 * @param[in]  t_max   The maximum parameter value
 * @param[out] rec     The hit record
 * @return     True if the ray hits the BVH node bounding box, False otherwise.
 */
bool BVHNode::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const
{
    if (!box.hit(r, t_min, t_max))
    {
        return false;
    }

    bool hit_left = left->hit(r, t_min, t_max, rec);
    bool hit_right = right->hit(r, t_min, hit_left ? rec.get_t() : t_max, rec);

    return hit_left || hit_right;
}

/**
 * @brief      Get the bounding box of the node.
 * @param[in]  time0       The time 0
 * @param[in]  time1       The time 1
 * @param[out]      output_box  The output box
 * @return     True if the bounding box is valid, False otherwise.
 */
bool BVHNode::bounding_box(double time0, double time1, AABB &output_box) const
{
    output_box = box;
    return true;
}

/**
 * @brief      Compares the bounding boxes of two objects.
 * @param[in]  a      The first object
 * @param[in]  b      The second object
 * @param[in]  axis   The axis
 * @return     True if the bounding box of the first object is smaller than the bounding box of the second object, False otherwise.
 */
bool box_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis)
{
    AABB box_a;
    AABB box_b;

    if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
    {
        std::cerr << "No bounding box in bvh_node constructor." << std::endl;
    }

    return box_a.get_min().e[axis] < box_b.get_min().e[axis];
}

/**
 * @brief      Compares the bounding boxes of two objects along the x axis.
 * @param[in]  a      The first object
 * @param[in]  b      The second object
 * @return     True if the bounding box of the first object is smaller than the bounding box of the second object, False otherwise.
 */

bool box_x_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
{
    return box_compare(a, b, 0);
}

/**
 * @brief      Compares the bounding boxes of two objects along the y axis.
 * @param[in]  a      The first object
 * @param[in]  b      The second object
 * @return     True if the bounding box of the first object is smaller than the bounding box of the second object, False otherwise.
 */
bool box_y_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
{
    return box_compare(a, b, 1);
}

/**
 * @brief      Compares the bounding boxes of two objects along the z axis.
 * @param[in]  a      The first object
 * @param[in]  b      The second object
 * @return     True if the bounding box of the first object is smaller than the bounding box of the second object, False otherwise.
 */
bool box_z_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
{
    return box_compare(a, b, 2);
}
