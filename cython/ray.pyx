include "vector.pyx"

cdef class Ray:
    """ray class """
    cdef public Vector origin
    cdef public Vector direction
    cdef public double time

    def __init__(self, Vector origin, Vector direction double time):
        self.origin = origin
        self.direction = direction
        self.time = time
        
    def __call__(self, t: float) -> Vector:
        """get position of ray at t"""
        return self.origin + self.direction*t

    def get_origin(self):
        return self.origin

    def get_direction(self):
        return self.direction