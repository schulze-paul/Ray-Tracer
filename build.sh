#!/bin/bash

# This script builds the cython binary and then runs the ray tracer

python3 cython_extension/setup.py build_ext --inplace

python3 main.py