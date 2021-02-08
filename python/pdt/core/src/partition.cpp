#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include "pdt/PartitionNode.hpp"

// Namespace resolution
namespace py = pybind11;

namespace pdt {
namespace python {

void
register_partition(py::module& m) {
  py::class_<pdt::PartitionNode, uhal::Node> (m, "PartitionNode")
      .def(py::init<const uhal::Node&>())
      .def("readTriggerMask", &pdt::PartitionNode::readTriggerMask)
      // .def("writeTriggerMask", &pdt::PartitionNode::writeTriggerMask)
      .def("configure", &pdt::PartitionNode::configure, py::arg("aTrigMask"), py::arg("aEnableSpillGate"), py::arg("aRateCtrl") = 1)
      .def("enableTriggers", &pdt::PartitionNode::enableTriggers, py::arg("aEnable") = true)
      .def("readBufferWordCount", &pdt::PartitionNode::readBufferWordCount)
      .def("numEventsInBuffer", &pdt::PartitionNode::numEventsInBuffer)
      .def("readROBWarningOverflow", &pdt::PartitionNode::readROBWarningOverflow)
      .def("readROBError", &pdt::PartitionNode::readROBError)
      .def("readEvents", &pdt::PartitionNode::readEvents, py::arg("aNumEvents") = 0)
      .def("enable", &pdt::PartitionNode::enable, py::arg("aEnable")=true,  py::arg("aDispatch")=true)
      .def("reset", &pdt::PartitionNode::reset)
      .def("start", &pdt::PartitionNode::start, py::arg("aTimeout") = 5000)
      .def("stop", &pdt::PartitionNode::stop, py::arg("aTimeout") = 5000)
      .def("configureRateCtrl", &pdt::PartitionNode::configureRateCtrl)
      .def("getStatus", &pdt::PartitionNode::getStatus, py::arg("aPrint")=false)
      ;
}

} // namespace python
} // namespace pdt