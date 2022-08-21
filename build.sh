#!/bin/bash
# Paul Schulze

# This script installs the required dependencies for growth_model
# and then builds the energy and gradient computation binary with cython


python3 cython/setup.py build_ext --inplace

python3 ray_tracer.py