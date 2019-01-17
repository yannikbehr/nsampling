CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(nsampling)
SET(PACKAGE_VERSION 0.2)

add_subdirectory(pybind11)
pybind11_add_module(nsampling src/nsampling_pybind11.cpp src/nested_sampling.cpp src/distributions.cpp)
