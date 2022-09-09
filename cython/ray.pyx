# cython: profile=True

include "vector.pyx"
cdef class Ray:
    """Class to store info about origin, direction and time of a light ray."""
    cdef public Vector origin
    cdef public Vector direction
    cdef public double time

    def __init__(self, Vector origin, Vector direction, double time):
        self.origin = origin
        self.direction = direction
        self.time = time
        
    cpdef Vector at(self, t: float):
        """Get position of ray at t."""
        return self.origin + self.direction*t
