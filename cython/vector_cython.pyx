

from libc.math cimport sqrt

import numpy as np
cimport numpy as np



cpdef double dot(Vector a, Vector b):
    return a.x() * b.x() + a.y() * b.y() + a.z() * b.z()


cdef class Vector:
    """class for 3d vectors"""

    cdef double x_val
    cdef double y_val
    cdef double z_val

    def __init__(self, double x, double y, double z):
        self.x_val = x
        self.y_val = y
        self.z_val = z

    cpdef double x(self):
        return self.x_val 

    cpdef double y(self):
        return self.y_val 

    cpdef double z(self):
        return self.z_val 

    @staticmethod
    def random(double min_value, double max_value) -> Vector:
        """get a random vector"""
        random_vals = np.random.uniform(min_value, max_value, 3)
        cdef double[:] random_vals_view = random_vals
        return Vector(random_vals_view[0], random_vals_view[1], random_vals_view[2])

    cpdef double length_sq(self):
        """get squared length of vector"""
        return self.x_val*self.x_val + self.y_val*self.y_val + self.z_val*self.z_val

    cpdef double length(self):
        """get length of vector"""
        return sqrt(self.length_sq())


    def __add__(self, Vector other):
        cdef double x = self.x() + other.x()
        cdef double y = self.y() + other.y()
        cdef double z = self.z() + other.z()

        return self.__class__(x, y, z)

    def __sub__(self, Vector other):
        cdef double x = self.x() - other.x()
        cdef double y = self.y() - other.y()
        cdef double z = self.z() - other.z()

        return self.__class__(x, y, z)

    def __mul__(self, double other):
        cdef double x = self.x() * other
        cdef double y = self.y() * other
        cdef double z = self.z() * other

        return self.__class__(x, y, z)

    def __truediv__(self, double other):
        cdef double x = self.x() / other
        cdef double y = self.y() / other
        cdef double z = self.z() / other

        return self.__class__(x, y, z)

    def __str__(self):
        return "["+ str(self.x()) +", "+ str(self.y()) +", "+ str(self.z()) +"]"

    def __eq__(self, other):
        if self.x() == other.x() and self.y() == other.y() and self.z() == other.z():
            return True
        else:
            return False



cdef class Color(Vector):
    """class to store rgb color information"""

    def __init__(self, double r, double g, double b):
        self.x_val = r
        self.y_val = g
        self.z_val = b

    def r(self):
        return self.__cut(self.x())

    def g(self):
        return self.__cut(self.y())

    def b(self):
        return self.__cut(self.z())

    cdef double __cut(self, double val):
        minimum = 0
        maximum = 0.999
        if val < minimum:
            return minimum
        if val > maximum:
            return maximum
        else:
            return val

    @staticmethod
    def from_vector(vec: Vector):
        return Color(vec.x(), vec.y(), vec.z())


cpdef Vector random_in_unit_sphere():
    while (True):
        point = Vector.random(-1, 1)
        if point.length_sq() > 1:
            pass
        else:
            return point
