/* File : Sampling.i */
%module(directors="1") sampling
%include "typemaps.i"
%include "std_string.i"

%{
#define SWIG_FILE_WITH_INIT
#include "distributions.h"
#include "nested_sampling.h"
%}

%include "carrays.i"
%array_class(double, d_array);

%feature("director") Callback;

%include "std_vector.i"
// Instantiate templates used by example
namespace std {
   %template(IntVector) vector<int>;
   %template(VarVector) vector<Variable*>;
   %template(DVector) vector<double>;
}

/* Let's just grab the original header file here */
%include "distributions.h"
%include "nested_sampling.h"