import numpy as np
import math


class Vector(np.ndarray):
    """class for 3d vectors"""
    def __new__(cls, x=0, y=0, z=0):
        return np.ndarray.__new__(cls, 3)

    def __init__(self, x=0, y=0, z=0):
        self[0] = x
        self[1] = y
        self[2] = z

    def x(self):
        return self[0]

    def y(self):
        return self[0]

    def z(self):
        return self[0]

    @staticmethod
    def random(min_value: float, max_value: float):
        random_vals = np.random.uniform(min_value, max_value, 3)
        return Vector(*random_vals)

    def length_sq(self) -> float:
        return self.x()**2 + self.y()**2 + self.z()**2

    def length(self) -> float:
        return math.sqrt(self.length_sq)


def dot(a: Vector, b: Vector) -> float:
    return np.dot(a, b)


class Color(np.ndarray):
    """class to store rgb color information"""
    def __new__(cls, r, g, b):
        return np.ndarray.__new__(cls, 3)

    def __init__(self, r, g, b):
        self[0] = r
        self[1] = g
        self[2] = b

    def r(self):
        return self.__cut(self[0])

    def g(self):
        return self.__cut(self[1])

    def b(self):
        return self.__cut(self[2])

    def __cut(x: float) -> float:
        minimum = 0
        maximum = 0.999
        if x < minimum:
            return minimum
        if x > maximum:
            return maximum
        else:
            return x


def random_in_unit_sphere() -> Vector:
    while (True):
        point = Vector.random(-1, 1)
        if point.length_sq() > 1:
            pass
        else:
            return point
