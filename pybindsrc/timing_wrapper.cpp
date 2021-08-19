/**
 * @file timing_wrapper.cpp
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

std::map<std::string, uint32_t> swap_commands_map(const std::map<uint32_t, std::string>& command_map) {
	std::map<std::string, uint32_t> swapped_map;

	for (auto it=command_map.begin(); it != command_map.end(); ++it) {
		swapped_map.emplace( std::pair<std::string,uint32_t>(it->second, it->first) );
	}
	return swapped_map;
}

namespace python {
	
	extern void register_i2c(py::module &);
	extern void register_partition(py::module &);
	extern void register_endpoint(py::module &);
	extern void register_io(py::module &);
	extern void register_master(py::module &);
	extern void register_top_designs(py::module &);
    extern void register_definitions(py::module &);

PYBIND11_MODULE(_daq_timing_py, top_module) {

    top_module.doc() = "c++ implementation of timing python modules"; // optional module docstring

    // timing.core
    py::module_ core_module = top_module.def_submodule("core");

	timing::python::register_i2c(core_module);
	timing::python::register_partition(core_module);
	timing::python::register_endpoint(core_module);
	timing::python::register_io(core_module);
	timing::python::register_master(core_module);
	timing::python::register_top_designs(core_module);

	// timing.common
	py::module_ common_module = top_module.def_submodule("common");

	// timing.common.definitions
	py::module_ defs_module = common_module.def_submodule("definitions");
    timing::python::register_definitions(defs_module);
}

} // namespace python
} // namespace timing
} // namespace dunedaq