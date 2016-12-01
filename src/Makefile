CFLAGS=-std=c++11 -g

all: interface library

interface:
	swig -python -c++ Sampling.i

library: distributions.h distributions.cpp
	g++ -fPIC $(CFLAGS) -c  distributions.cpp -I./;\
	g++ -fPIC $(CFLAGS) -c  nested_sampling.cpp -I./;\
	g++ -fPIC  $(CFLAGS) -c Sampling_wrap.cxx -I/usr/include/python2.7 -I./;\
	g++ -shared $(CFLAGS) distributions.o nested_sampling.o Sampling_wrap.o -o _sampling.so


lighthouse: example.cpp nested_sampling.h nested_sampling.cpp
	g++ -o example -I ./ example.cpp nested_sampling.cpp
	
clean:
	-rm *.o *.so *.cxx *.pyc