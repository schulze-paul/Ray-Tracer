# Ray Tracer

![Preview image](https://github.com/schulze-paul/ray-tracer/blob/main/images/image_800_steady.png?raw=true)

Inspired by Peter Shirley's [book series](https://raytracing.github.io) about ray tracing.

## Motivation

I have always been fascinated with the way that light interacts with matter and shapes the way we see and experience our world. What I found intriguing about ray tracing is how the simple premise of accurately modelling the light path can lead to cosmetically very appealing and realistic looking images.

I had the opportunity to work with state of the art optical equipment in the lab during my [bachelor's thesis](https://github.com/schulze-paul/BA-THESIS-Coherent-Light-Simulations), where I also simulated optical characteristics of coherent light. In this context, the interactions shown here are relatively simple. Since the computational cost of ray tracing is relatively high though, I was a challenge for me to create a highly performant algorithm for this use case, this is especially true given that the program is written in python.

## Features
- Fresnel reflection
- Refraction based on Snell's law
- Diffuse scattering
- Motion blur
- Soft shadows
- Depth of field
- Custom C extension built with Cython
- Multi-core parallelization
- Bounding Volume Hierarchy

## Optimizations

### Custom C Extension
I created a custom Python extension in C using [Cython](https://cython.org/) to handle the bulk of the computational expenses.

This includes a 3D vector extension class with corresponding operators as well as dot and cross product. It also includes a base class for rays and the hit detection.  
### Parallelization

<div>
  <img width=350 src=https://github.com/schulze-paul/ray-tracer/blob/feature/speedup/images/parallel_time.png>
  <img width=350 src=https://github.com/schulze-paul/ray-tracer/blob/feature/speedup/images/parallel_speedup.png>
</div>

**Figure 1: Execution time and speedup using parallelization.**

Ray tracing is well suited for parallelization. I used the multiprocessing library to distribute the number of rays per pixel among the available cpu cores. The speedup is significant with each additional process until 9 cores where a platau of about 3.5x is hit.

### Bounding Volume Hierarchy

### Planned: Scattering PDF
