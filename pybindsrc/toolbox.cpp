/**
 * @file toolbox.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/toolbox.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace dunedaq {
namespace timing {
namespace python {

void
register_toolbox(py::module& m)
{
  m.def("format_firmware_version", &timing::format_firmware_version);
}

} // namespace python
} // namespace timing
} // namespace dunedaq
