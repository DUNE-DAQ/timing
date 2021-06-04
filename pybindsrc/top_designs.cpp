/**
 * @file top_designs.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/FMCIONode.hpp"
#include "timing/PC059IONode.hpp"
#include "timing/TLUIONode.hpp"
#include "timing/FIBIONode.hpp"

#include "timing/PDIMasterNode.hpp"

#include "timing/BoreasDesign.hpp"
#include "timing/FanoutDesign.hpp"
#include "timing/OuroborosMuxDesign.hpp"
#include "timing/OverlordDesign.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace dunedaq {
namespace timing {
namespace python {

void
register_top_designs(py::module& m)
{

  // Overlord on TLU
  py::class_<timing::OverlordDesign<TLUIONode>, uhal::Node>(m, "OverlordDesign<TLUIONode>")
    .def("get_status", &timing::OverlordDesign<TLUIONode>::get_status)
    .def("apply_endpoint_delay", &timing::OverlordDesign<TLUIONode>::apply_endpoint_delay)
    .def("measure_endpoint_rtt", &timing::OverlordDesign<TLUIONode>::measure_endpoint_rtt)
    .def("get_external_triggers_endpoint_node",
         &timing::OverlordDesign<TLUIONode>::get_external_triggers_endpoint_node);

  // Overlord on FMC
  py::class_<timing::OverlordDesign<FMCIONode>, uhal::Node>(m, "OverlordDesign<FMCIONode>")
    .def("get_status", &timing::OverlordDesign<FMCIONode>::get_status)
    .def("apply_endpoint_delay", &timing::OverlordDesign<FMCIONode>::apply_endpoint_delay)
    .def("measure_endpoint_rtt", &timing::OverlordDesign<FMCIONode>::measure_endpoint_rtt)
    .def("get_external_triggers_endpoint_node",
         &timing::OverlordDesign<FMCIONode>::get_external_triggers_endpoint_node);

  // Boreas on FMC
  py::class_<timing::BoreasDesign<FMCIONode>, uhal::Node>(m, "BoreasDesign<FMCIONode>")
    .def("get_status", &timing::BoreasDesign<FMCIONode>::get_status)
    .def("apply_endpoint_delay", &timing::BoreasDesign<FMCIONode>::apply_endpoint_delay)
    .def("measure_endpoint_rtt", &timing::BoreasDesign<FMCIONode>::measure_endpoint_rtt);

  // Boreas on TLU
  py::class_<timing::BoreasDesign<TLUIONode>, uhal::Node>(m, "BoreasDesign<TLUIONode>")
    .def("get_status", &timing::BoreasDesign<TLUIONode>::get_status)
    .def("apply_endpoint_delay", &timing::BoreasDesign<TLUIONode>::apply_endpoint_delay)
    .def("measure_endpoint_rtt", &timing::BoreasDesign<TLUIONode>::measure_endpoint_rtt);

  // Boreas on sim
  py::class_<timing::BoreasDesign<SIMIONode>, uhal::Node>(m, "BoreasDesign<SIMIONode>")
    .def("get_status", &timing::BoreasDesign<SIMIONode>::get_status)
    .def("apply_endpoint_delay", &timing::BoreasDesign<SIMIONode>::apply_endpoint_delay)
    .def("measure_endpoint_rtt", &timing::BoreasDesign<SIMIONode>::measure_endpoint_rtt);

  // PD-I fanout design on pc059
  py::class_<timing::FanoutDesign<PC059IONode, PDIMasterNode>, uhal::Node>(m, "FanoutDesign<PC059IONode,PDIMasterNode>")
    .def("switch_sfp_mux_channel", &timing::FanoutDesign<PC059IONode, PDIMasterNode>::switch_sfp_mux_channel)
    .def("apply_endpoint_delay", &timing::FanoutDesign<PC059IONode, PDIMasterNode>::apply_endpoint_delay)
    .def("measure_endpoint_rtt", &timing::FanoutDesign<PC059IONode, PDIMasterNode>::measure_endpoint_rtt)
    .def("scan_sfp_mux", &timing::FanoutDesign<PC059IONode, PDIMasterNode>::scan_sfp_mux);

  // PD-I fanout design on fib
  py::class_<timing::FanoutDesign<FIBIONode, PDIMasterNode>, uhal::Node>(m, "FanoutDesign<FIBIONode,PDIMasterNode>")
    .def("switch_sfp_mux_channel", &timing::FanoutDesign<FIBIONode, PDIMasterNode>::switch_sfp_mux_channel)
    .def("apply_endpoint_delay", &timing::FanoutDesign<FIBIONode, PDIMasterNode>::apply_endpoint_delay)
    .def("measure_endpoint_rtt", &timing::FanoutDesign<FIBIONode, PDIMasterNode>::measure_endpoint_rtt)
    .def("scan_sfp_mux", &timing::FanoutDesign<FIBIONode, PDIMasterNode>::scan_sfp_mux);

  // PD-I ouroboros design on pc059
  py::class_<timing::OuroborosMuxDesign<PC059IONode>, uhal::Node>(
    m, "OuroborosMuxDesign<PC059IONode>")
    .def("switch_sfp_mux_channel", &timing::OuroborosMuxDesign<PC059IONode>::switch_sfp_mux_channel)
    .def("apply_endpoint_delay", &timing::OuroborosMuxDesign<PC059IONode>::apply_endpoint_delay)
    .def("measure_endpoint_rtt", &timing::OuroborosMuxDesign<PC059IONode>::measure_endpoint_rtt)
    .def("scan_sfp_mux", &timing::OuroborosMuxDesign<PC059IONode>::scan_sfp_mux);

  // PD-I ouroboros design on fib
  py::class_<timing::OuroborosMuxDesign<FIBIONode>, uhal::Node>(
    m, "OuroborosMuxDesign<FIBIONode>")
    .def("switch_sfp_mux_channel", &timing::OuroborosMuxDesign<FIBIONode>::switch_sfp_mux_channel)
    .def("apply_endpoint_delay", &timing::OuroborosMuxDesign<FIBIONode>::apply_endpoint_delay)
    .def("measure_endpoint_rtt", &timing::OuroborosMuxDesign<FIBIONode>::measure_endpoint_rtt)
    .def("scan_sfp_mux", &timing::OuroborosMuxDesign<FIBIONode>::scan_sfp_mux);
}

} // namespace python
} // namespace timing
} // namespace dunedaq