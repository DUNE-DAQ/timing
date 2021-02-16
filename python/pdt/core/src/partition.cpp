#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include "pdt/PartitionNode.hpp"

// Namespace resolution
namespace py = pybind11;

namespace dunedaq {
namespace pdt {
namespace python {

void
register_partition(py::module& m) {
  py::class_<pdt::PartitionNode, uhal::Node> (m, "PartitionNode")
      .def(py::init<const uhal::Node&>())
      .def("read_trigger_mask", &pdt::PartitionNode::read_trigger_mask)
      // .def("writeTriggerMask", &pdt::PartitionNode::writeTriggerMask)
      .def("configure", &pdt::PartitionNode::configure, py::arg("aTrigMask"), py::arg("aEnableSpillGate"), py::arg("aRateCtrl") = 1)
      .def("enable_triggers", &pdt::PartitionNode::enable_triggers, py::arg("aEnable") = true)
      .def("read_buffer_word_count", &pdt::PartitionNode::read_buffer_word_count)
      .def("num_events_in_buffer", &pdt::PartitionNode::num_events_in_buffer)
      .def("read_rob_warning_overflow", &pdt::PartitionNode::read_rob_warning_overflow)
      .def("read_rob_error", &pdt::PartitionNode::read_rob_error)
      .def("read_events", &pdt::PartitionNode::read_events, py::arg("aNumEvents") = 0)
      .def("enable", &pdt::PartitionNode::enable, py::arg("aEnable")=true,  py::arg("aDispatch")=true)
      .def("reset", &pdt::PartitionNode::reset)
      .def("start", &pdt::PartitionNode::start, py::arg("aTimeout") = 5000)
      .def("stop", &pdt::PartitionNode::stop, py::arg("aTimeout") = 5000)
      .def("configure_rate_ctrl", &pdt::PartitionNode::configure_rate_ctrl)
      .def("get_status", &pdt::PartitionNode::get_status, py::arg("aPrint")=false)
      ;
}

} // namespace python
} // namespace pdt
} // namespace dunedaq