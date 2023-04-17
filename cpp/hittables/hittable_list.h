#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include <vector>
#include <memory>
#include "hittable.h"

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
    std::vector<std::shared_ptr<Hittable>> getObjects() const
    {
        return objects;
    }

private:
    std::vector<std::shared_ptr<Hittable>> objects;
};

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
            closest_so_far = temp_rec.getT();
            rec = temp_rec;
        }
    }

    return hit_anything;
}

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


#endif // HITTABLE_LIST_H