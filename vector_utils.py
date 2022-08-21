import numpy as np
import math

from vector_cython import Vector


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

    def __cut(self, x: float) -> float:
        minimum = 0
        maximum = 0.999
        if x < minimum:
            return minimum
        if x > maximum:
            return maximum
        else:
            return x

    @staticmethod
    def from_vector(vec: Vector):
        return Color(vec.x(), vec.y(), vec.z())


def random_in_unit_sphere() -> Vector:
    while (True):
        point = Vector.random(-1, 1)
        if point.length_sq() > 1:
            pass
        else:
            return point
