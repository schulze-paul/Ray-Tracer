

from libc.math cimport sqrt, fabs, fmin

import numpy as np
cimport numpy as np



cpdef double dot(Vector a, Vector b):
    """Dot product of two vectors a and b: a_x*b_x + a_y*b_y + a_z*b_z."""
    return a.x * b.x + a.y * b.y + a.z * b.z


cdef class Vector:
    """Class to store coordinates for 3d vectors."""

    cdef public double x
    cdef public double y
    cdef public double z

    def __init__(self, double x, double y, double z):
        self.x = x
        self.y = y
        self.z = z


    @staticmethod
    def random(double min_value, double max_value) -> Vector:
        """Get a random vector where all entries are between min_value and max_value."""
        random_vals = np.random.uniform(min_value, max_value, 3)
        cdef double[:] random_vals_view = random_vals
        return Vector(random_vals_view[0], random_vals_view[1], random_vals_view[2])

    cpdef double length_sq(self):
        """Get squared length of vector."""
        return self.x*self.x + self.y*self.y + self.z*self.z

    cpdef double length(self):
        """Get length of vector."""
        return sqrt(self.length_sq())


    def __add__(self, Vector other):
        """Add two vectors."""
        cdef double x = self.x + other.x
        cdef double y = self.y + other.y
        cdef double z = self.z + other.z

        return self.__class__(x, y, z)

    def __sub__(self, Vector other):
        """Subtract two vectors."""
        cdef double x = self.x - other.x
        cdef double y = self.y - other.y
        cdef double z = self.z - other.z

        return self.__class__(x, y, z)

    def __mul__(self, double other):
        """Multiply all vector elements with a double value."""
        cdef double x = self.x * other
        cdef double y = self.y * other
        cdef double z = self.z * other

        return self.__class__(x, y, z)

    def __truediv__(self, double other):
        """Divide all vector elements by a double value."""
        cdef double x = self.x / other
        cdef double y = self.y / other
        cdef double z = self.z / other

        return self.__class__(x, y, z)

    def __str__(self):
        """Print out the coordinates of the vector."""
        return "["+ str(self.x) +", "+ str(self.y) +", "+ str(self.z) +"]"

    def __eq__(self, other):
        """Check if two vectors have the same coordinates."""
        if self.x == other.x and self.y == other.y and self.z == other.z:
            return True
        else:
            return False
            
    cpdef near_zero(self):
        """Check if all vector elements are close to zero."""
        cdef double eps = 1e-8
        
        return fabs(self.x) < eps and fabs(self.y) < eps and fabs(self.z) < eps 


cpdef Vector outer(Vector vector_a, Vector vector_b):
    """Get the outer product c of two vectors a and b: c_i = a_i * b_i"""
    cdef double x = vector_a.x * vector_b.x
    cdef double y = vector_a.y * vector_b.y
    cdef double z = vector_a.z * vector_b.z
    return Vector(x, y, z)

cpdef Vector reflect(Vector vector_in, Vector normal):
    """
    Reflect an incoming ray at a surface and get the outcoming ray direction with the laws of reflection.
    
    Parameters:
    -----------
    vector_in: Vector
        Incoming ray direction.
    normal: Vector
        direction of the surface normal at the reflection point
    """
    return vector_in + normal * - 2*dot(vector_in, normal)


cdef class Color(Vector):
    """Class to store rgb color Information"""

    def __init__(self, double r, double g, double b):
        self.x = r
        self.y = g
        self.z = b

    cpdef r(self):
        """Get red value of color."""
        return self.__cut(self.x)

    cpdef g(self):
        """Get green value of color."""
        return self.__cut(self.y)

    cpdef b(self):
        """Get blue value of color."""
        return self.__cut(self.z)

    cdef double __cut(self, double val):
        """TODO: remove"""
        minimum = 0
        maximum = 0.999
        if val < minimum:
            return minimum
        if val > maximum:
            return maximum
        else:
            return val

    @staticmethod
    def from_vector(vec: Vector) -> Color:
        """Convert a vector to an rgb color: x -> red, y -> green, z -> blue."""
        return Color(vec.x, vec.y, vec.z)


cpdef Vector random_in_unit_sphere():
    """Get a random point inside a sphere with radius 1 (uniform distribution)."""
    while (True):
        point = Vector.random(-1, 1)
        if point.length_sq() > 1:
            pass
        else:
            return point

cpdef Vector unit_vector(Vector vector):
    """Get the direction of a vector with length 1."""
    return vector / vector.length()

cpdef Vector random_unit_vector():
    """Get a random vector of length 1."""
    return unit_vector(random_in_unit_sphere())
    
cpdef Vector random_in_hemisphere(Vector normal):
    """
    Get a random point in the ouside part of a surface point surrounding sphere.
    
    Assumes locally flat surface. Uniform distribution.

    Parameters:
    -----------
    normal: Vector
        The normal vector of the surface point.
    """
    
    cdef Vector in_unit_sphere = random_in_unit_sphere()
    
    # check if point is inside volume or outside 
    if dot(in_unit_sphere, normal) > 0.0:
        return in_unit_sphere
    else:
        return -in_unit_sphere
        
cpdef Vector random_in_unit_disk():
    """Get a random point inside a circulat disk of radius 1."""
    while(True):
        point = Vector.random(-1, 1)
        point = Vector(point.x, point.y, 0)
        if point.length_sq() > 1:
            pass
        else:
            return point


cpdef Vector refract(Vector vector_in, Vector normal, double refractive_indeces_fraction):
    """
    Refract an incoming ray at an interface between two media.
    
    Compute the direction of a refracted light ray at the interface with Snell's law. 

    Paramters:
    ----------
    vector_in: Vector
        Direction of the incoming ray.
    normal: Vector
        normal Vector
    refractive_indeces_fraction: double
        n_in / n_out 
    
    Returns:
    --------
    Vector:
        direction of the outcoming ray. 
    """
    
    cdef double cos_theta = fmin(dot(vector_in*-1, normal), 1.0)
    cdef Vector out_perpendicular = (vector_in + normal * cos_theta) * refractive_indeces_fraction
    cdef Vector out_parallel = normal * -sqrt(fabs(1.0-out_perpendicular.length_sq()))
    return out_perpendicular + out_parallel


cpdef Vector cross(Vector vector_a, Vector vector_b):
    """
    Get the cross product of two vectors:

    c = a cross b: 
    
    c_x = a_y * b_z - a_z * b_y
    c_y = a_z * b_x - a_x * b_z
    c_z = a_x * b_y - a_y * b_x
    """
    
    cdef x = vector_a.y * vector_b.z - vector_a.z * vector_b.y
    cdef y = vector_a.z * vector_b.x - vector_a.x * vector_b.z
    cdef z = vector_a.x * vector_b.y - vector_a.y * vector_b.x
    return Vector(x, y, z)