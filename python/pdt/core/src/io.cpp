#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "pdt/IONode.hpp"
#include "pdt/FMCIONode.hpp"
#include "pdt/PC059IONode.hpp"
#include "pdt/TLUIONode.hpp"
#include "pdt/SIMIONode.hpp"

namespace py = pybind11;

namespace dunedaq {
namespace pdt {
namespace python {

void
register_io(py::module& m) {
      
      py::class_<pdt::IONode, uhal::Node> (m, "IONode")
      ;
      
      py::class_<pdt::FMCIONode, pdt::IONode, uhal::Node> (m, "FMCIONode")
        .def(py::init<const uhal::Node&>())
        .def("reset", &pdt::FMCIONode::reset, py::arg("aClockConfigFile") = "")
        .def("soft_reset", &pdt::FMCIONode::soft_reset)
        .def("get_clock_frequencies_table", &pdt::FMCIONode::get_clock_frequencies_table, py::arg("aPrint") = false)
        .def("get_status", &pdt::FMCIONode::get_status, py::arg("aPrint") = false)
        .def("get_pll_status", &pdt::FMCIONode::get_pll_status, py::arg("aPrint") = false)
        .def("get_hardware_info", &pdt::FMCIONode::get_hardware_info, py::arg("aPrint") = false)
        .def("get_sfp_status", &pdt::FMCIONode::get_sfp_status, py::arg("aSFPId"), py::arg("aPrint") = false)
        .def("switch_sfp_soft_tx_control_bit", &pdt::FMCIONode::switch_sfp_soft_tx_control_bit)
      ;

      py::class_<pdt::PC059IONode, pdt::IONode, uhal::Node> (m, "PC059IONode")
        .def(py::init<const uhal::Node&>())
        .def<void (pdt::PC059IONode::*)(const std::string&) const>("reset", &pdt::PC059IONode::reset, py::arg("aClockConfigFile") = "")
        .def<void (pdt::PC059IONode::*)(int32_t,const std::string&) const>("reset", &pdt::PC059IONode::reset, py::arg("aFanoutMode"), py::arg("aClockConfigFile") = "")
        .def("soft_reset", &pdt::PC059IONode::soft_reset)
        .def("get_clock_frequencies_table", &pdt::PC059IONode::get_clock_frequencies_table, py::arg("aPrint") = false)
        .def("get_status", &pdt::PC059IONode::get_status, py::arg("aPrint") = false)
        .def("get_pll_status", &pdt::PC059IONode::get_pll_status, py::arg("aPrint") = false)
        .def("get_hardware_info", &pdt::PC059IONode::get_hardware_info, py::arg("aPrint") = false)
        .def("get_sfp_status", &pdt::PC059IONode::get_sfp_status, py::arg("aSFPId"), py::arg("aPrint") = false)
        .def("switch_sfp_soft_tx_control_bit", &pdt::PC059IONode::switch_sfp_soft_tx_control_bit)
        .def("switch_sfp_mux_channel", &pdt::PC059IONode::switch_sfp_mux_channel)
        .def("read_active_sfp_mux_channel", &pdt::PC059IONode::read_active_sfp_mux_channel)
      ;

      py::class_<pdt::TLUIONode, pdt::IONode, uhal::Node> (m, "TLUIONode")
        .def(py::init<const uhal::Node&>())
        .def("reset", &pdt::TLUIONode::reset, py::arg("aClockConfigFile") = "")
        .def("soft_reset", &pdt::TLUIONode::soft_reset)
        .def("get_clock_frequencies_table", &pdt::TLUIONode::get_clock_frequencies_table, py::arg("aPrint") = false)
        .def("get_status", &pdt::TLUIONode::get_status, py::arg("aPrint") = false)
        .def("get_pll_status", &pdt::TLUIONode::get_pll_status, py::arg("aPrint") = false)
        .def("get_hardware_info", &pdt::TLUIONode::get_hardware_info, py::arg("aPrint") = false)
        .def("get_sfp_status", &pdt::TLUIONode::get_sfp_status, py::arg("aSFPId"), py::arg("aPrint") = false)
        .def("switch_sfp_soft_tx_control_bit", &pdt::TLUIONode::switch_sfp_soft_tx_control_bit)
        .def("configure_dac", &pdt::TLUIONode::configure_dac, py::arg("aDACId"), py::arg("aDACValue"), py::arg("aInternalRef") = false)
        ;
  
      py::class_<pdt::SIMIONode, pdt::IONode, uhal::Node> (m, "SIMIONode")
        .def(py::init<const uhal::Node&>())
        .def("reset", &pdt::SIMIONode::reset, py::arg("aClockConfigFile") = "")
        .def("soft_reset", &pdt::SIMIONode::soft_reset)
        .def("get_clock_frequencies_table", &pdt::SIMIONode::get_clock_frequencies_table, py::arg("aPrint") = false)
        .def("get_status", &pdt::SIMIONode::get_status, py::arg("aPrint") = false)
        .def("get_pll_status", &pdt::SIMIONode::get_pll_status, py::arg("aPrint") = false)
        .def("get_hardware_info", &pdt::SIMIONode::get_hardware_info, py::arg("aPrint") = false)
        .def("get_sfp_status", &pdt::SIMIONode::get_sfp_status, py::arg("aSFPId"), py::arg("aPrint") = false)
        .def("switch_sfp_soft_tx_control_bit", &pdt::SIMIONode::switch_sfp_soft_tx_control_bit)
        ;

}

} // namespace python
} // namespace pdt
} // namespace dunedaq