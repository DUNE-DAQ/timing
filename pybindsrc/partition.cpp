/**
 * @file partition.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/PartitionNode.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

// Namespace resolution
namespace py = pybind11;

namespace dunedaq {
namespace timing {
namespace python {

void
register_partition(py::module& m)
{
  py::class_<timing::PartitionNode, uhal::Node>(m, "PartitionNode")
    .def(py::init<const uhal::Node&>())
    .def("read_trigger_mask", &timing::PartitionNode::read_trigger_mask)
    // .def("writeTriggerMask", &timing::PartitionNode::writeTriggerMask)
    .def("configure",
         &timing::PartitionNode::configure,
         py::arg("trigger_mask"),
         py::arg("enableSpillGate"),
         py::arg("rate_control_enabled") = 1)
    .def("enable_triggers", &timing::PartitionNode::enable_triggers, py::arg("enable") = true)
    .def("read_buffer_word_count", &timing::PartitionNode::read_buffer_word_count)
    .def("num_events_in_buffer", &timing::PartitionNode::num_events_in_buffer)
    .def("read_rob_warning_overflow", &timing::PartitionNode::read_rob_warning_overflow)
    .def("read_rob_error", &timing::PartitionNode::read_rob_error)
    .def("read_events", &timing::PartitionNode::read_events, py::arg("number_of_events") = 0)
    .def("enable", &timing::PartitionNode::enable, py::arg("enable") = true, py::arg("dispatch") = true)
    .def("reset", &timing::PartitionNode::reset)
    .def("start", &timing::PartitionNode::start, py::arg("timeout") = 5000)
    .def("stop", &timing::PartitionNode::stop, py::arg("timeout") = 5000)
    .def("configure_rate_ctrl", &timing::PartitionNode::configure_rate_ctrl)
    .def("get_status", &timing::PartitionNode::get_status, py::arg("print_out") = false);
}

} // namespace python
} // namespace timing
} // namespace dunedaq