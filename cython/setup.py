
from setuptools import setup
from Cython.Build import cythonize
import numpy as np


import Cython.Compiler.Options
Cython.Compiler.Options.annotate = True

setup(name='Computation', ext_modules=cythonize(["cython/ray.pyx"],
                                                annotate=True,
                                                compiler_directives={'language_level': "3"}),
      include_dirs=[np.get_include()],
      zip_safe=False,
      )
