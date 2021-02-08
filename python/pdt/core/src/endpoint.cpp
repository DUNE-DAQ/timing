#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "pdt/EndpointNode.hpp"
#include "pdt/CRTNode.hpp"

namespace py = pybind11;

namespace pdt {
namespace python {

void
register_endpoint(py::module& m) {
  py::class_<pdt::EndpointNode, uhal::Node> (m, "EndpointNode")
      .def(py::init<const uhal::Node&>())
      .def("disable", &pdt::EndpointNode::disable)
      .def("enable", &pdt::EndpointNode::enable, py::arg("partition") = 0, py::arg("address") = 0)
      .def("reset", &pdt::EndpointNode::reset, py::arg("partition") = 0, py::arg("address") = 0)
      .def("readBufferCount", &pdt::EndpointNode::readBufferCount)
      .def("readDataBuffer", &pdt::EndpointNode::readDataBuffer, py::arg("aReadall") = false)
      .def("getDataBufferTable", &pdt::EndpointNode::getDataBufferTable, py::arg("aReadall") = false, py::arg("aPrint") = false)
      .def("readVersion", &pdt::EndpointNode::readVersion)
      .def("readTimestamp", &pdt::EndpointNode::readTimestamp)
      .def("readClockFrequency", &pdt::EndpointNode::readClockFrequency)
      ;

  py::class_<pdt::CRTNode, uhal::Node> (m, "CRTNode")
      .def(py::init<const uhal::Node&>())
      .def("disable", &pdt::CRTNode::disable)
      .def("enable", &pdt::CRTNode::enable)
      .def("getStatus", &pdt::CRTNode::getStatus, py::arg("aPrint") = false)
      .def("readLastPulseTimestamp", &pdt::CRTNode::readLastPulseTimestamp)
      ;
}

} // namespace python
} // namespace pdt