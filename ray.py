# local imports
from dataclasses import dataclass

from vector_cython import Vector


@dataclass
class Ray():
    """ray class """
    origin: Vector
    direction: Vector

    def __call__(self, t: float) -> Vector:
        """get position of ray at t"""
        return self.origin + self.direction*t
