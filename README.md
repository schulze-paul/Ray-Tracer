# Fast Ray Tracer in Python. 

![Preview image](https://github.com/schulze-paul/ray-tracer/blob/main/images/rendered_image.png?raw=true)


Inspired by Peter Shirley's [book series](https://raytracing.github.io) about ray tracing.

The way that light interacts with matter and shapes the way we see and experience our world is fascinating. The beauty of ray tracing is that it is based on the physical processes of light-matter interactions. I studied coherent light interactions in detail during my [bachelor's thesis](https://github.com/schulze-paul/BA-Thesis). However the simulations are very specialized to examine a specific effect and are not capable to render any images of 3D objects.

- Fresnel reflections
- Refractions based on Snell's law
- Diffuse scattering
- Motion blur
- Soft shadows
- Depth of field
- [Custom C extension](#custom-c-extension) built with Cython
- [Multi core processing](#multi-core-processing)
- [Bounding volume hierarchy](#bounding-volume-hierarchy)

## Installation
Requires [Python 3.10](https://www.python.org/) and [pip](https://pypi.org/project/pip/).  
Run `./install.sh` (Linux) or `install.bat` (Windows).
This will install the needed [dependencies](https://github.com/schulze-paul/Ray-Tracer/blob/main/requirements.txt) and build the `C extension` for your system.


## Optimizations

### Custom C Extension

<img width=500 src="https://github.com/schulze-paul/Ray-Tracer/blob/main/images/cython_speedup.png?raw=true">


The Ray Tracer uses a custom C extension which is built with [`Cython`](https://cython.org/). It handles the bulk of the computational expenses like vector operations, reflections, refractions and hit detection. 


### Multi core processing

<div>
  <img width=350 src=https://github.com/schulze-paul/ray-tracer/blob/main/images/parallel_time.png>
  <img width=350 src=https://github.com/schulze-paul/ray-tracer/blob/main/images/parallel_speedup.png>
</div>

**Execution time and speedup using parallelization.**

Ray tracing is well suited for parallelization. The multiprocessing library distributes the number of rays per pixel among the available cpu cores. The speedup is significant with each additional process until 9 cores where a platau of about 3.5x is hit.

### Bounding Volume Hierarchy
The Ray Tracer uses axis aligned bounding boxes and generates a bounding volume hierarchy at intialization. This improves performance significantly for a scene with many objects.
