





cdef class Ray():
    """ray class """
    cdef origin: Vector
    direction: Vector

    def __call__(self, t: float) -> Vector:
        """get position of ray at t"""
        return self.origin + t*self.direction