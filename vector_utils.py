from mimetypes import init
import numpy as np


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


def length(vec: Vector) -> float:
    return vec.x()**2 + vec.y()**2 + vec.z()**2


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
        return self[0]

    def g(self):
        return self[0]

    def b(self):
        return self[0]
