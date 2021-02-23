#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "pdt/EndpointNode.hpp"
#include "pdt/CRTNode.hpp"

namespace py = pybind11;

namespace dunedaq {
namespace pdt {
namespace python {

void
register_endpoint(py::module& m) {
  py::class_<pdt::EndpointNode, uhal::Node> (m, "EndpointNode")
      .def(py::init<const uhal::Node&>())
      .def("disable", &pdt::EndpointNode::disable)
      .def("enable", &pdt::EndpointNode::enable, py::arg("partition") = 0, py::arg("address") = 0)
      .def("reset", &pdt::EndpointNode::reset, py::arg("partition") = 0, py::arg("address") = 0)
      .def("read_buffer_count", &pdt::EndpointNode::read_buffer_count)
      .def("read_data_buffer", &pdt::EndpointNode::read_data_buffer, py::arg("read_all") = false)
      .def("get_data_buffer_table", &pdt::EndpointNode::get_data_buffer_table, py::arg("read_all") = false, py::arg("print_out") = false)
      .def("read_version", &pdt::EndpointNode::read_version)
      .def("read_timestamp", &pdt::EndpointNode::read_timestamp)
      .def("read_clock_frequency", &pdt::EndpointNode::read_clock_frequency)
      ;

  py::class_<pdt::CRTNode, uhal::Node> (m, "CRTNode")
      .def(py::init<const uhal::Node&>())
      .def("disable", &pdt::CRTNode::disable)
      .def("enable", &pdt::CRTNode::enable)
      .def("get_status", &pdt::CRTNode::get_status, py::arg("print_out") = false)
      .def("read_last_pulse_timestamp", &pdt::CRTNode::read_last_pulse_timestamp)
      ;
}

} // namespace python
} // namespace pdt
} // namespace dunedaq