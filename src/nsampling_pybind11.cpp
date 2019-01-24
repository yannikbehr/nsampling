#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include "distributions.h"
#include "nested_sampling.h"

namespace py = pybind11;

PYBIND11_MODULE(nsampling, m){
	// Distributions
	py::class_<Variable, std::shared_ptr<Variable> >(m, "Variable");
	py::class_<Constant, Variable, std::shared_ptr<Constant> >(m, "Constant")
		.def(py::init<std::string, double>())
		.def(py::init<const Constant &>())
		.def("draw", &Constant::draw)
		.def("trial", &Constant::trial)
		.def("get_name", &Constant::get_name, "Get the variable name.")
		.def("get_value", &Constant::get_value, "Get the variable value.")
		.def("clone", &Constant::clone, "Return a clone of the current instance.");
	py::class_<Normal, Variable, std::shared_ptr<Normal> >(m, "Normal")
		.def(py::init<std::string, double, double>())
		.def(py::init<const Normal &>())
		.def("draw", &Normal::draw)
		.def("trial", &Normal::trial)
		.def("get_name", &Normal::get_name, "Get the variable name.")
		.def("get_value", &Normal::get_value, "Get the variable value.")
		.def("clone", &Normal::clone, "Return a clone of the current instance.");
	py::class_<CUniform, Variable, std::shared_ptr<CUniform> >(m, "CUniform")
		.def(py::init<std::string, double, double>())
		.def(py::init<const CUniform &>())
		.def("draw", &CUniform::draw)
		.def("trial", &CUniform::trial)
		.def("get_name", &CUniform::get_name, "Get the variable name.")
		.def("get_value", &CUniform::get_value, "Get the variable value.")
		.def("clone", &CUniform::clone, "Return a clone of the current instance.");
	py::class_<Uniform, Variable, std::shared_ptr<Uniform> >(m, "Uniform")
		.def(py::init<std::string, double, double>())
		.def(py::init<const Uniform &>())
		.def("draw", &Uniform::draw)
		.def("trial", &Uniform::trial)
		.def("get_name", &Uniform::get_name, "Get the variable name.")
		.def("get_value", &Uniform::get_value, "Get the variable value.")
		.def("clone", &Uniform::clone, "Return a clone of the current instance.");
	
	// Nested Sampling
	py::class_<Object, std::shared_ptr<Object> >(m, "Object")
		.def(py::init<std::vector<std::shared_ptr<Variable> > >())
	        .def(py::init<Object &>())
	        .def("draw", &Object::draw)
		.def("trial", &Object::trial)
		.def("get_logL", &Object::get_logL)
		.def("get_logWt", &Object::get_logWt)
		.def("get_value", &Object::get_value)
		.def("get_logZ", &Object::get_logZ)
		.def("get_H", &Object::get_H)
	        .def("assign", &Object::operator=, py::is_operator());
	py::class_<Result>(m, "Result")
		.def(py::init<std::vector<std::shared_ptr<Object> >,double, double, int>())
		.def("getexpt", &Result::getexpt)
		.def("getvar", &Result::getvar)
		.def("getmax", &Result::getmax)
		.def("getname", &Result::getnames)
		.def("getZ", &Result::getZ)
		.def("getH", &Result::getH)
		.def("get_samples", &Result::get_samples)
		.def("resample_posterior", &Result::resample_posterior);
	py::class_<NestedSampling>(m, "NestedSampling")
		.def(py::init<>())
		.def("explore", &NestedSampling::explore, py::arg("vars"),
				py::arg("initial_samples"),
				py::arg("maximum_steps"),
				py::arg("likelihood"),
				py::arg("mcmc_steps") = 20,
				py::arg("stepscale") = 0.1,
				py::arg("tolZ") = 1e-3);

}
