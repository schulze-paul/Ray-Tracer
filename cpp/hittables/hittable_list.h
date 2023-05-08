/*
Hittable List
=============

The HittableList class contains a list of hittable objects.
*/

#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include <vector>
#include <memory>
#include "hittable.h"
#include "hit_record.h"


/**
 * @brief      Class for hittable list.
 */
class HittableList : public Hittable
{
public:
    int list_size;

    HittableList() {}
    HittableList(std::shared_ptr<Hittable> object)
    {
        add(object);
    }
    int add(std::shared_ptr<Hittable> object)
    {
        objects.push_back(object);
        return 0;
    }
    int size() const
    {
        return objects.size();
    }
    virtual bool hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const;
    virtual bool bounding_box(double t0, double t1, AABB &box) const;
    std::shared_ptr<Hittable> get(int i) const
    {
        return objects[i];
    }
    std::vector<std::shared_ptr<Hittable>> get_objects() const
    {
        return objects;
    }
    std::shared_ptr<Hittable> get_object(int i) const
    {
        return objects[i];
    }
    std::string to_string() const
    {
        return "HittableList";
    }
    virtual Vec3 random(const Vec3 &o) const;

private:
    std::vector<std::shared_ptr<Hittable>> objects;
};

/**
 * @brief      Determines if the ray hits any of the objects in the list.
 * @param[in]  r       The ray
 * @param[in]  t_min   The minimum distance along the ray
 * @param[in]  t_max   The maximum distance along the ray
 * @param[out] rec     The hit record
 * @return     True if the ray hits any of the objects in the list, False otherwise.
 */
bool HittableList::hit(const Ray &r, double t_min, double t_max, HitRecord &rec) const
{
    HitRecord temp_rec;
    bool hit_anything = false;
    double closest_so_far = t_max;

    for (const auto &object : objects)
    {
        if (object->hit(r, t_min, closest_so_far, temp_rec))
        {
            hit_anything = true;
            closest_so_far = temp_rec.get_t();
            rec = temp_rec;
        }
    }

    return hit_anything;
}

/**
 * @brief      Constructs an AABB that contains all of the objects in the list.
 * @param[in]  t0    The start time
 * @param[in]  t1    The end time
 * @param[out] box   The AABB
 * @return     True if the AABB was constructed, False otherwise.
 */
bool HittableList::bounding_box(double t0, double t1, AABB &box) const
{
    if (objects.empty())
        return false;

    AABB temp_box;
    bool first_true = objects[0]->bounding_box(t0, t1, temp_box);
    if (!first_true)
        return false;
    else
        box = temp_box;

    for (const auto &object : objects)
    {
        if (!object->bounding_box(t0, t1, temp_box))
            return false;
        box = surrounding_box(box, temp_box);
    }

    return true;
}

Vec3 HittableList::random(const Vec3 &o) const
{
    int index = random_int(0, list_size - 1);
    return objects[index]->random(o);
}
#endif // HITTABLE_LIST_H