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

extern void
register_i2c(py::module&);
extern void
register_partition(py::module&);
extern void
register_endpoint(py::module&);
extern void
register_io(py::module&);
extern void
register_master(py::module&);
extern void
register_top_designs(py::module&);

PYBIND11_MODULE(_core, m)
{

  m.doc() = "c++ implementation of timing core python module"; // optional module docstring

  timing::python::register_i2c(m);
  timing::python::register_partition(m);
  timing::python::register_endpoint(m);
  timing::python::register_io(m);
  timing::python::register_master(m);
  timing::python::register_top_designs(m);
}

} // namespace python
} // namespace timing
} // namespace dunedaq