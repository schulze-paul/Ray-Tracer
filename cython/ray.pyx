include "vector.pyx"

cdef class Ray:
    """ray class """
    cdef public Vector origin
    cdef public Vector direction

    def __init__(self, Vector origin, Vector direction):
        self.origin = origin
        self.direction = direction
        
    def __call__(self, t: float) -> Vector:
        """get position of ray at t"""
        return self.origin + self.direction*t

    def get_origin(self):
        return self.origin

    def get_direction(self):
        return self.direction