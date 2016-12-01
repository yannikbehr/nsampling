![](https://raw.githubusercontent.com/yannikbehr/sampling/master/examples/lighthouse_problem.png)
# Sampling
Sampling is a C++ library with a Python interface intended to implement various
importance sampling algorithms. At the moment it only implements Nested Sampling
as published by Sivia and Skilling [1].

As it uses swig to generate the Python interface
many other high-level language interfaces could be supported.
## Installation

### Dependencies
* cmake (>=3.5)
* swig (>=3.0)

### Compiling the library and python extension module
```
mkdir build
cd build
cmake ..
make install
```

### Installing the python extension module
```
cd python
python setup.py install
```

### Compiling and running the C++ example
```
cd examples
make
make run
```

### Running the example Jupyter notebook
To run the Jupyter notebook you have to have `numpy` and `matplotlib` installed
in addition to `jupyter`.

```
cd examples
jupyter notebook&
``` 
