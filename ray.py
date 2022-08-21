import numpy as np

# local imports
from dataclasses import dataclass


@dataclass
class Ray():
    """ray class """
    origin: np.ndarray
    direction: np.ndarray

    def __call__(self, t: float) -> np.ndarray:
        """get position of ray at t"""
        return self.origin + t*self.direction
