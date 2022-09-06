# Ray Tracer

![Preview image](https://github.com/schulze-paul/ray-tracer/blob/main/images/image_800_steady.png?raw=true)

Inspired by Peter Shirley's [book series](https://raytracing.github.io) about ray tracing.

## Motivation

I have always been fascinated with the way that light interacts with matter and shapes the way we see and experience our world. What I found intriguing about ray tracing is how the simple premise of accurately modelling the light path can lead to consmetically very appealing and realistic looking images.

I had the opportunity to work with state of the art optical equipment in the lab during my [bachelor's thesis](https://github.com/schulze-paul/BA-THESIS-Coherent-Light-Simulations), where I also simulated optical characteristics of coherent light. In this context, the interactions shown here are relatively simple. Since the computational cost of ray tracing is relatively high though, I was a challenge for me to create a highly performant algorithm for this use case, this is especially true given that the program is written in python.

## Optical effects
- Fresnel reflection
- Refraction based on Snell's law
- Diffuse scattering
- Motion blur
- Soft shadows
- Depth of field

## Optimizations

### Custom Extension
I created a custom Python extension in C using [Cython](https://cython.org/) to handle the bulk of the computational expenses.

### Paralellization
Parallelization is relatively easyly accomplished. 

