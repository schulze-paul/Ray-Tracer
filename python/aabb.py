from sphere import Hittable, Vector, Ray


class AxisAlignedBoundingBox(Hittable):

    def __init__(self, point_a: Vector, point_b: Vector):
        min_x = min(point_a.x(), point_b.x())
        min_y = min(point_a.y(), point_b.y())
        min_z = min(point_a.z(), point_b.z())

        max_x = max(point_a.x(), point_b.x())
        max_y = max(point_a.y(), point_b.y())
        max_z = max(point_a.z(), point_b.z())

        self.minimum = Vector(min_x, min_y, min_z)
        self.maximum = Vector(max_x, max_y, max_z)

    def is_hit(self, ray, t_min, t_max, t_hit):

        for a in range(3):
            divisior = 1/ray.direction[a]

            t0 = min((self.minimum[a] - ray.origin[a])/divisior,
                     (self.maximum[a] - ray.origin[a])/divisior,)
            t1 = max((self.minimum[a] - ray.origin[a])/divisior,
                     (self.maximum[a] - ray.origin[a])/divisior,)

            if divisior < 0:
                temp = t0
                t0 = t1
                t1 = temp

            if t0 > t_min:
                t_min = t0
            if t1 < t_max:
                t_max = t1

            if t_max <= t_min:
                return False

        return True

    def bounding_box(self, time0, time1):
        return self
