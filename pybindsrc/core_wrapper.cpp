/**
 * @file core_wrapper.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace dunedaq {
namespace timing {
namespace python {
	
	extern void register_i2c(py::module &);
	extern void register_partition(py::module &);
	extern void register_endpoint(py::module &);
	extern void register_io(py::module &);
	extern void register_master(py::module &);
	extern void register_top_designs(py::module &);

PYBIND11_MODULE(_daq_timing_py, top_module) {

    top_module.doc() = "c++ implementation of timing python modules"; // optional module docstring

    py::module_ core_module = top_module.def_submodule("core");

	timing::python::register_i2c(core_module);
	timing::python::register_partition(core_module);
	timing::python::register_endpoint(core_module);
	timing::python::register_io(core_module);
	timing::python::register_master(core_module);
	timing::python::register_top_designs(core_module);

}

} // namespace python
} // namespace timing
} // namespace dunedaq