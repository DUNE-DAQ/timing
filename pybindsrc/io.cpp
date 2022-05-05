/**
 * @file io.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/FMCIONode.hpp"
#include "timing/IONode.hpp"
#include "timing/PC059IONode.hpp"
#include "timing/FIBIONode.hpp"
#include "timing/SIMIONode.hpp"
#include "timing/TLUIONode.hpp"
#include "timing/MIBIONode.hpp"
#include "timing/SwitchyardNode.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>

namespace py = pybind11;

namespace dunedaq {
namespace timing {
namespace python {

void
register_io(py::module& m)
{

  py::class_<timing::IONode, uhal::Node>(m, "IONode");

  py::class_<timing::FMCIONode, timing::IONode, uhal::Node>(m, "FMCIONode")
    .def(py::init<const uhal::Node&>())
    .def<void (timing::FMCIONode::*)(const std::string&) const>(
      "reset", &timing::FMCIONode::reset, py::arg("clock_config_file") = "")
    .def("soft_reset", &timing::FMCIONode::soft_reset)
    .def("read_firmware_frequency", &timing::FMCIONode::read_firmware_frequency)
    .def("get_clock_frequencies_table", &timing::FMCIONode::get_clock_frequencies_table, py::arg("print_out") = false)
    .def("get_status", &timing::FMCIONode::get_status, py::arg("print_out") = false)
    .def("get_pll_status", &timing::FMCIONode::get_pll_status, py::arg("print_out") = false)
    .def("get_hardware_info", &timing::FMCIONode::get_hardware_info, py::arg("print_out") = false)
    .def("get_sfp_status", &timing::FMCIONode::get_sfp_status, py::arg("sfp_id"), py::arg("print_out") = false)
    .def("switch_sfp_soft_tx_control_bit", &timing::FMCIONode::switch_sfp_soft_tx_control_bit);

  py::class_<timing::PC059IONode, timing::IONode, uhal::Node>(m, "PC059IONode")
    .def(py::init<const uhal::Node&>())
    .def<void (timing::PC059IONode::*)(const std::string&) const>(
      "reset", &timing::PC059IONode::reset, py::arg("clock_config_file") = "")
    .def<void (timing::PC059IONode::*)(int32_t, const std::string&) const>(
      "reset", &timing::PC059IONode::reset, py::arg("fanout_mode"), py::arg("clock_config_file") = "")
    .def("soft_reset", &timing::PC059IONode::soft_reset)
    .def("read_firmware_frequency", &timing::PC059IONode::read_firmware_frequency)
    .def("get_clock_frequencies_table", &timing::PC059IONode::get_clock_frequencies_table, py::arg("print_out") = false)
    .def("get_status", &timing::PC059IONode::get_status, py::arg("print_out") = false)
    .def("get_pll_status", &timing::PC059IONode::get_pll_status, py::arg("print_out") = false)
    .def("get_hardware_info", &timing::PC059IONode::get_hardware_info, py::arg("print_out") = false)
    .def("get_sfp_status", &timing::PC059IONode::get_sfp_status, py::arg("sfp_id"), py::arg("print_out") = false)
    .def("switch_sfp_soft_tx_control_bit", &timing::PC059IONode::switch_sfp_soft_tx_control_bit)
    .def("switch_downstream_mux_channel", &timing::PC059IONode::switch_downstream_mux_channel, py::arg("mux_channel"))
    .def("read_active_downstream_mux_channel", &timing::PC059IONode::read_active_downstream_mux_channel);

    py::class_<timing::FIBIONode, timing::IONode, uhal::Node>(m, "FIBIONode")
    .def(py::init<const uhal::Node&>())
    .def<void (timing::FIBIONode::*)(const std::string&) const>(
      "reset", &timing::FIBIONode::reset, py::arg("clock_config_file") = "")
    .def<void (timing::FIBIONode::*)(int32_t, const std::string&) const>(
      "reset", &timing::FIBIONode::reset, py::arg("fanout_mode"), py::arg("clock_config_file") = "")
    .def("soft_reset", &timing::FIBIONode::soft_reset)
    .def("read_firmware_frequency", &timing::FIBIONode::read_firmware_frequency)
    .def("get_clock_frequencies_table", &timing::FIBIONode::get_clock_frequencies_table, py::arg("print_out") = false)
    .def("get_status", &timing::FIBIONode::get_status, py::arg("print_out") = false)
    .def("get_pll_status", &timing::FIBIONode::get_pll_status, py::arg("print_out") = false)
    .def("get_hardware_info", &timing::FIBIONode::get_hardware_info, py::arg("print_out") = false)
    .def("get_sfp_status", &timing::FIBIONode::get_sfp_status, py::arg("sfp_id"), py::arg("print_out") = false)
    .def("switch_sfp_soft_tx_control_bit", &timing::FIBIONode::switch_sfp_soft_tx_control_bit)
    .def("switch_downstream_mux_channel", &timing::FIBIONode::switch_downstream_mux_channel, py::arg("mux_channel"))
    .def("read_active_downstream_mux_channel", &timing::FIBIONode::read_active_downstream_mux_channel);

  py::class_<timing::TLUIONode, timing::IONode, uhal::Node>(m, "TLUIONode")
    .def(py::init<const uhal::Node&>())
    .def<void (timing::TLUIONode::*)(const std::string&) const>(
      "reset", &timing::TLUIONode::reset, py::arg("clock_config_file") = "")
    .def("soft_reset", &timing::TLUIONode::soft_reset)
    .def("read_firmware_frequency", &timing::TLUIONode::read_firmware_frequency)
    .def("get_clock_frequencies_table", &timing::TLUIONode::get_clock_frequencies_table, py::arg("print_out") = false)
    .def("get_status", &timing::TLUIONode::get_status, py::arg("print_out") = false)
    .def("get_pll_status", &timing::TLUIONode::get_pll_status, py::arg("print_out") = false)
    .def("get_hardware_info", &timing::TLUIONode::get_hardware_info, py::arg("print_out") = false)
    .def("get_sfp_status", &timing::TLUIONode::get_sfp_status, py::arg("sfp_id"), py::arg("print_out") = false)
    .def("switch_sfp_soft_tx_control_bit", &timing::TLUIONode::switch_sfp_soft_tx_control_bit)
    .def("configure_dac",
         &timing::TLUIONode::configure_dac,
         py::arg("dac_id"),
         py::arg("dac_value"),
         py::arg("internal_ref") = false);

  py::class_<timing::SIMIONode, timing::IONode, uhal::Node>(m, "SIMIONode")
    .def(py::init<const uhal::Node&>())
    .def<void (timing::SIMIONode::*)(const std::string&) const>(
      "reset", &timing::SIMIONode::reset, py::arg("clock_config_file") = "")
    .def("soft_reset", &timing::SIMIONode::soft_reset)
    .def("read_firmware_frequency", &timing::SIMIONode::read_firmware_frequency)
    .def("get_clock_frequencies_table", &timing::SIMIONode::get_clock_frequencies_table, py::arg("print_out") = false)
    .def("get_status", &timing::SIMIONode::get_status, py::arg("print_out") = false)
    .def("get_pll_status", &timing::SIMIONode::get_pll_status, py::arg("print_out") = false)
    .def("get_hardware_info", &timing::SIMIONode::get_hardware_info, py::arg("print_out") = false)
    .def("get_sfp_status", &timing::SIMIONode::get_sfp_status, py::arg("sfp_id"), py::arg("print_out") = false)
    .def("switch_sfp_soft_tx_control_bit", &timing::SIMIONode::switch_sfp_soft_tx_control_bit);

  py::class_<timing::MIBIONode, timing::IONode, uhal::Node>(m, "MIBIONode")
    .def(py::init<const uhal::Node&>())
    .def<void (timing::MIBIONode::*)(const std::string&) const>(
      "reset", &timing::MIBIONode::reset, py::arg("clock_config_file") = "")
    .def<void (timing::MIBIONode::*)(int32_t, const std::string&) const>(
      "reset", &timing::MIBIONode::reset, py::arg("fanout_mode"), py::arg("clock_config_file") = "")
    .def("soft_reset", &timing::MIBIONode::soft_reset)
    .def("read_firmware_frequency", &timing::MIBIONode::read_firmware_frequency)
    .def("get_clock_frequencies_table", &timing::MIBIONode::get_clock_frequencies_table, py::arg("print_out") = false)
    .def("get_status", &timing::MIBIONode::get_status, py::arg("print_out") = false)
    .def("get_pll_status", &timing::MIBIONode::get_pll_status, py::arg("print_out") = false)
    .def("get_pll", &timing::MIBIONode::get_pll)
    .def("get_hardware_info", &timing::MIBIONode::get_hardware_info, py::arg("print_out") = false)
    .def("get_sfp_status", &timing::MIBIONode::get_sfp_status, py::arg("sfp_id"), py::arg("print_out") = false)
    .def("switch_sfp_soft_tx_control_bit", &timing::MIBIONode::switch_sfp_soft_tx_control_bit)
    .def("switch_downstream_mux_channel", &timing::MIBIONode::switch_downstream_mux_channel, py::arg("mux_channel"))
    .def("read_active_downstream_mux_channel", &timing::MIBIONode::read_active_downstream_mux_channel);

    py::class_<timing::SwitchyardNode, uhal::Node>(m, "SwitchyardNode")
      .def("get_status", &timing::SwitchyardNode::get_status, py::arg("print_out") = false)
      .def("configure_master_source", &timing::SwitchyardNode::configure_master_source, py::arg("master_source"), py::arg("dispatch") = true)
      .def("configure_endpoint_source", &timing::SwitchyardNode::configure_endpoint_source, py::arg("endpoint_source"), py::arg("dispatch") = true);

} // NOLINT

} // namespace python
} // namespace timing
} // namespace dunedaq