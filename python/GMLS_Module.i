%{
#define SWIG_FILE_WITH_INIT
%}
/*%include numpy.i*/
%include std_string.i
%include exception.i
%init %{
import_array();
%}

%define DOCSTRING
"The GMLS_Module provides all features for accessing functionality of the C++ Compadre Toolkit. This includes neighbor search, setting up and solving the quadratic programs produced by the GMLS approach, as well as the application of these solutions with data. It is important to initializeKokkos() before calling any other functionality, and to call finalizeKokkos() when you are finished."
%enddef

%module(docstring=DOCSTRING) GMLS_Module
%{
#include "GMLS_Python.hpp"
#include "Compadre_GMLS.hpp"
#include "Kokkos_Core.hpp"
#include "Python.h"
#include "numpy/arrayobject.h"
%}

%exception { 
    try {
        $action
    } catch(std::exception &_e) {
        SWIG_exception_fail(SWIG_SystemError, (&_e)->what());
    } catch (...) {
        SWIG_exception(SWIG_RuntimeError, "unknown exception");
    }
}

/*%feature("docstring") finalizeKokkos() "example for other comments."*/

/* Parse the header file to generate wrappers */
%feature("autodoc", "1");

%include "GMLS_Python.hpp"
