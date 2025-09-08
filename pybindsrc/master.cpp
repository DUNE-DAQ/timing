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
#include "timing/PhaseMeasurementNode.hpp"

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
    .def("read_endpoint_data", &timing::MasterNode::read_endpoint_data
    , py::arg("endpoint_address"), py::arg("reg_address"), py::arg("data_length"), py::arg("address_mode") 
    , py::arg("timeout") = 500)
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
    .def("read_timestamp", &timing::MasterNode::read_timestamp)
    .def("disable_timestamp_broadcast", &timing::MasterNode::disable_timestamp_broadcast)
    .def("enable_timestamp_broadcast", &timing::MasterNode::enable_timestamp_broadcast)
    .def("configure_endpoint_command_decoder", &timing::MasterNode::configure_endpoint_command_decoder,
     py::arg("endpoint_address"),
     py::arg("slot"),
     py::arg("command"))
    .def<uint32_t (timing::MasterNode::*)(uint16_t, uint16_t, uint8_t, bool) const>("measure_endpoint_rtt",
      &timing::MasterNode::measure_endpoint_rtt,
      py::arg("address"),
      py::arg("fanout_ept_address"),
      py::arg("fanout_mux"),
      py::arg("control_sfp") = true)
    .def<uint32_t (timing::MasterNode::*)(uint16_t, bool) const>("measure_endpoint_rtt",
      &timing::MasterNode::measure_endpoint_rtt,
      py::arg("address"),
      py::arg("control_sfp") = true)
    .def("apply_endpoint_delay",
      &timing::MasterNode::apply_endpoint_delay,
      py::arg("address"),
      py::arg("cycle_delay"),
      py::arg("phase_delay")
      )
    .def("resync_endpoint",
      &timing::MasterNode::resync_endpoint,
      py::arg("address")
      )
    .def("set_fanout_mux",
      &timing::MasterNode::set_fanout_mux,
      py::arg("fanout_ept_adr"),
      py::arg("fanout_mux")
      )
    ;

  py::class_<timing::UpstreamCDRNode, uhal::Node>(m, "UpstreamCDRNode")
    .def(py::init<const uhal::Node&>())
    .def("get_status", &timing::UpstreamCDRNode::get_status, py::arg("print_out") = false)
    .def("resync", &timing::UpstreamCDRNode::resync);

  py::class_<timing::IRIGTimestampNode, uhal::Node>(m, "IRIGTimestampNode")
    .def(py::init<const uhal::Node&>())
    .def("get_status", &timing::IRIGTimestampNode::get_status, py::arg("print_out") = false)
    .def("set_ts_timebase", &timing::IRIGTimestampNode::set_ts_timebase, py::arg("ts_timebase"))
    .def("set_ts_epoch", &timing::IRIGTimestampNode::set_ts_epoch, py::arg("ts_epoch"))
    .def("set_ts_epoch_value", &timing::IRIGTimestampNode::set_ts_epoch_value, py::arg("epoch_to_2000_seconds_tai"),  py::arg("epoch_to_2000_leap_seconds"))
    .def("set_ts_seconds_offset", &timing::IRIGTimestampNode::set_ts_seconds_offset, py::arg("seconds_offset"))
    .def("set_ts_ticks_offset", &timing::IRIGTimestampNode::set_ts_ticks_offset, py::arg("ticks_offset"))
    ;

  py::class_<timing::PhaseMeasurementNode, uhal::Node>(m, "PhaseMeasurementNode")
    .def(py::init<const uhal::Node&>())
    .def("measure_phase", &timing::PhaseMeasurementNode::measure_phase, py::arg("ref_clk") = 255)
    .def("measure_phases", &timing::PhaseMeasurementNode::measure_phases)
    ;

}

} // namespace python
} // namespace timing
} // namespace dunedaq
