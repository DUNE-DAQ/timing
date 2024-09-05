/**
 * @file master.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/MasterNode.hpp"
#include "timing/UpstreamCDRNode.hpp"
#include "timing/IRIGTimestampNode.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace dunedaq {
namespace timing {
namespace python {

void
register_master(py::module& m)
{
  py::class_<timing::MasterNode, uhal::Node>(m, "MasterNode")
    .def(py::init<const uhal::Node&>())
    .def("switch_endpoint_sfp", &timing::MasterNode::switch_endpoint_sfp)
    .def("enable_upstream_endpoint", &timing::MasterNode::enable_upstream_endpoint)
    .def("reset_command_counters", &timing::MasterNode::reset_command_counters)
    .def("transmit_async_packet", &timing::MasterNode::transmit_async_packet, py::arg("packet"), py::arg("timeout") = 500) //timeout [us]
    .def("write_endpoint_data", &timing::MasterNode::write_endpoint_data)
    .def("read_endpoint_data", &timing::MasterNode::read_endpoint_data)
    .def("send_fl_cmd",
         &timing::MasterNode::send_fl_cmd,
         py::arg("command"),
         py::arg("channel"),
         py::arg("number_of_commands") = 1)
    .def<void (timing::MasterNode::*)(uint32_t, uint32_t, double, bool, uint32_t) const>("enable_periodic_fl_cmd",
         &timing::MasterNode::enable_periodic_fl_cmd,
         py::arg("command"),
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"),
         py::arg("clock_frequency_hz"))
    .def("disable_periodic_fl_cmd", &timing::MasterNode::disable_periodic_fl_cmd)
    .def("get_status", &timing::MasterNode::get_status, py::arg("print_out") = false)
    .def("get_status_with_date", &timing::MasterNode::get_status_with_date, py::arg("clock_frequency_hz"), py::arg("print_out") = false)
    .def("sync_timestamp", &timing::MasterNode::sync_timestamp, py::arg("source"))
    .def("disable_timestamp_broadcast", &timing::MasterNode::disable_timestamp_broadcast)
    .def("enable_timestamp_broadcast", &timing::MasterNode::enable_timestamp_broadcast)
    .def("configure_endpoint_command_decoder", &timing::MasterNode::configure_endpoint_command_decoder,
     py::arg("endpoint_address"),
     py::arg("slot"),
     py::arg("command"));

  py::class_<timing::UpstreamCDRNode, uhal::Node>(m, "UpstreamCDRNode")
    .def(py::init<const uhal::Node&>())
    .def("get_status", &timing::UpstreamCDRNode::get_status, py::arg("print_out") = false)
    .def("resync", &timing::UpstreamCDRNode::resync);

  py::class_<timing::IRIGTimestampNode, uhal::Node>(m, "IRIGTimestampNode")
    .def(py::init<const uhal::Node&>())
    .def("get_status", &timing::IRIGTimestampNode::get_status, py::arg("print_out") = false)
    .def("set_irig_epoch", &timing::IRIGTimestampNode::set_irig_epoch, py::arg("irig_epoch"))
    ;

}

} // namespace python
} // namespace timing
} // namespace dunedaq