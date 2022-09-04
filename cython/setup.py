from setuptools import setup
from Cython.Build import cythonize
import numpy as np

# print html files
import Cython.Compiler.Options
Cython.Compiler.Options.annotate = True

# compile cython files to a C extension
setup(name='Computation', ext_modules=cythonize(["cython/hittable.pyx"],
                                                annotate=True,
                                                compiler_directives={'language_level': "3"}),
      include_dirs=[np.get_include()],
      zip_safe=False,
      )
