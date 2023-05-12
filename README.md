
# Ray Tracing 

![title image](./images/title.png ).

Inspired by Peter Shirley's [book series](https://raytracing.github.io) about ray tracing.

The way that light interacts with matter and shapes the way we see and experience our world is fascinating. The beauty of ray tracing is that it is only based on the physical processes of light-matter interactions. I studied coherent light interactions in detail during my [bachelor's thesis](https://github.com/schulze-paul/BA-Thesis). However the simulations are very specialized to examine a specific effect and are not capable to render any images of 3D objects.

Working with this project, I was able to use many of the skills developed doing scientific computing and get a very visually appealing result:

I started the implementation in Python with Numpy: 
- I created a custom Python extension in C++ using Cython to handle handlevector operations, reflections, refractions and hit detection.
- Using the multiprocessing library, I sped up the execution further.

Not satisfied with the result, I ported the whole project to C++, and implemented additional features:
- Axis-aligned bounding boxes: Hit detection is sped up by first checking if a ray hits the bounding box containing the object.
- Bounding volume hierarchy nodes: The scene is split into a tree of bounding boxes, which are then traversed to find the closest hit.
- Probability Distribution functions: The rays are distributed according to a cosine distribution to simulate diffuse reflection. Light sources are sampled according to their intensity to reduce noise.

## Input files

The scene is described in a YAML file, which is parsed with the help of the [yaml-cpp](www.github.com/jbeder/yaml-cpp) library. It contains objects, materials, light sources and the camera setup.

## Spectral Rendering
I am working on a spectral renderer, which simulates the wavelength-dependent behaviour of light. This is useful for simulating the appearance of materials like glass, water or metals. The spectral color is represented by a 81 dimensional vector, which is sampled at 81 wavelengths between 380nm and 780nm. The 




