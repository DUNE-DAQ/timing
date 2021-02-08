#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace pdt {
namespace python {
	
	extern void register_i2c(py::module &);
	extern void register_partition(py::module &);
	extern void register_endpoint(py::module &);
	extern void register_io(py::module &);
	extern void register_master(py::module &);
	extern void register_top_designs(py::module &);

PYBIND11_MODULE(core, m) {

    m.doc() = "pdt core pybind11 module"; // optional module docstring

	pdt::python::register_i2c(m);
	pdt::python::register_partition(m);
	pdt::python::register_endpoint(m);
	pdt::python::register_io(m);
	pdt::python::register_master(m);
	pdt::python::register_top_designs(m);

}
}
}