/* File : Sampling.i */
%module(directors="1") sampling
%include "typemaps.i"
%include "std_string.i"
%include "std_except.i"
%include "pyabc.i"

%{
#define SWIG_FILE_WITH_INIT
#include "distributions.h"
#include "nested_sampling.h"
%}

%feature("director") Callback;

// Python requires that anything we raise inherits from this
%pythonabc(SamplingException, Exception);

%feature("director:except") {
    PyObject *etype = $error;
    if (etype != NULL) {
      PyObject *obj, *trace;
      PyErr_Fetch(&etype, &obj, &trace);
      Py_DecRef(etype);
      Py_DecRef(trace);
      // Not too sure if I need to call Py_DecRef for obj

      void *ptr;
      int res = SWIG_ConvertPtr(obj, &ptr, SWIGTYPE_p_SamplingException, 0);
      if (SWIG_IsOK(res) && ptr) {
        SamplingException *e = reinterpret_cast< SamplingException * >(ptr);
        // Throw by pointer (Yucky!)
        throw e;
      }

      throw Swig::DirectorMethodException();
    }
}

%include "std_vector.i"
// Instantiate templates used by example
namespace std {
   %template(IntVector) vector<int>;
   %template(VarVector) vector<Variable*>;
   %template(OVector) vector<Object*>;
   %template(DVector) vector<double>;
}

/* Let's just grab the original header file here */
%include "distributions.h"
%include "nested_sampling.h"
