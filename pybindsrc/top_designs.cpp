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
#include "timing/ChronosDesign.hpp"
#include "timing/CRTDesign.hpp"
#include "timing/FanoutDesign.hpp"
#include "timing/OuroborosDesign.hpp"
#include "timing/OuroborosMuxDesign.hpp"
#include "timing/OverlordDesign.hpp"
#include "timing/EndpointDesign.hpp"

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
    .def("read_firmware_version", &timing::OverlordDesign<TLUIONode>::read_firmware_version)
    .def("validate_firmware_version", &timing::OverlordDesign<TLUIONode>::validate_firmware_version)
    .def("sync_timestamp", &timing::OverlordDesign<TLUIONode>::sync_timestamp)
    .def("get_status", &timing::OverlordDesign<TLUIONode>::get_status)
    .def("enable_fake_trigger",
         &timing::OverlordDesign<TLUIONode>::enable_fake_trigger,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("apply_endpoint_delay", &timing::OverlordDesign<TLUIONode>::apply_endpoint_delay)
    .def("measure_endpoint_rtt", &timing::OverlordDesign<TLUIONode>::measure_endpoint_rtt)
    .def("get_external_triggers_endpoint_node",
         &timing::OverlordDesign<TLUIONode>::get_external_triggers_endpoint_node)
    .def("get_endpoint_node", &timing::OverlordDesign<TLUIONode>::get_endpoint_node);

  // Overlord on FMC
  py::class_<timing::OverlordDesign<FMCIONode>, uhal::Node>(m, "OverlordDesign<FMCIONode>")
    .def("read_firmware_version", &timing::OverlordDesign<FMCIONode>::read_firmware_version)
    .def("validate_firmware_version", &timing::OverlordDesign<FMCIONode>::validate_firmware_version)
    .def("sync_timestamp", &timing::OverlordDesign<FMCIONode>::sync_timestamp)
    .def("get_status", &timing::OverlordDesign<FMCIONode>::get_status)
    .def("enable_fake_trigger",
         &timing::OverlordDesign<FMCIONode>::enable_fake_trigger,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("apply_endpoint_delay", &timing::OverlordDesign<FMCIONode>::apply_endpoint_delay)
    .def("measure_endpoint_rtt", &timing::OverlordDesign<FMCIONode>::measure_endpoint_rtt)
    .def("get_external_triggers_endpoint_node",
         &timing::OverlordDesign<FMCIONode>::get_external_triggers_endpoint_node)
    .def("get_endpoint_node", &timing::OverlordDesign<FMCIONode>::get_endpoint_node);

  // Boreas on FMC
  py::class_<timing::BoreasDesign<FMCIONode>, uhal::Node>(m, "BoreasDesign<FMCIONode>")
    .def("read_firmware_version", &timing::BoreasDesign<FMCIONode>::read_firmware_version)
    .def("validate_firmware_version", &timing::BoreasDesign<FMCIONode>::validate_firmware_version)
    .def("sync_timestamp", &timing::BoreasDesign<FMCIONode>::sync_timestamp)
    .def("get_status", &timing::BoreasDesign<FMCIONode>::get_status)
    .def("enable_fake_trigger",
         &timing::BoreasDesign<FMCIONode>::enable_fake_trigger,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("apply_endpoint_delay", &timing::BoreasDesign<FMCIONode>::apply_endpoint_delay)
    .def("measure_endpoint_rtt", &timing::BoreasDesign<FMCIONode>::measure_endpoint_rtt)
    .def("get_hsi_node", &timing::BoreasDesign<FMCIONode>::get_hsi_node)
    .def("configure_hsi", 
         &timing::BoreasDesign<FMCIONode>::configure_hsi,
         py::arg("src"),
         py::arg("re_mask"),
         py::arg("fe_mask"),
         py::arg("inv_mask"),
         py::arg("rate"),
         py::arg("dispatch") = true);

  // Boreas on TLU
  py::class_<timing::BoreasDesign<TLUIONode>, uhal::Node>(m, "BoreasDesign<TLUIONode>")
    .def("read_firmware_version", &timing::BoreasDesign<TLUIONode>::read_firmware_version)
    .def("validate_firmware_version", &timing::BoreasDesign<TLUIONode>::validate_firmware_version)
    .def("sync_timestamp", &timing::BoreasDesign<TLUIONode>::sync_timestamp)
    .def("get_status", &timing::BoreasDesign<TLUIONode>::get_status)
    .def("enable_fake_trigger",
         &timing::BoreasDesign<TLUIONode>::enable_fake_trigger,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("apply_endpoint_delay", &timing::BoreasDesign<TLUIONode>::apply_endpoint_delay)
    .def("measure_endpoint_rtt", &timing::BoreasDesign<TLUIONode>::measure_endpoint_rtt)
    .def("get_hsi_node", &timing::BoreasDesign<TLUIONode>::get_hsi_node)
    .def("configure_hsi", 
         &timing::BoreasDesign<TLUIONode>::configure_hsi,
         py::arg("src"),
         py::arg("re_mask"),
         py::arg("fe_mask"),
         py::arg("inv_mask"),
         py::arg("rate"),
         py::arg("dispatch") = true);

  // Boreas on sim
  py::class_<timing::BoreasDesign<SIMIONode>, uhal::Node>(m, "BoreasDesign<SIMIONode>")
    .def("read_firmware_version", &timing::BoreasDesign<SIMIONode>::read_firmware_version)
    .def("validate_firmware_version", &timing::BoreasDesign<SIMIONode>::validate_firmware_version)
    .def("sync_timestamp", &timing::BoreasDesign<SIMIONode>::sync_timestamp)
    .def("get_status", &timing::BoreasDesign<SIMIONode>::get_status)
    .def("enable_fake_trigger",
         &timing::BoreasDesign<SIMIONode>::enable_fake_trigger,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("apply_endpoint_delay", &timing::BoreasDesign<SIMIONode>::apply_endpoint_delay)
    .def("measure_endpoint_rtt", &timing::BoreasDesign<SIMIONode>::measure_endpoint_rtt)
    .def("get_hsi_node", &timing::BoreasDesign<SIMIONode>::get_hsi_node)
    .def("configure_hsi", 
         &timing::BoreasDesign<SIMIONode>::configure_hsi,
         py::arg("src"),
         py::arg("re_mask"),
         py::arg("fe_mask"),
         py::arg("inv_mask"),
         py::arg("rate"),
         py::arg("dispatch") = true);

  // PD-I fanout design on pc059
  py::class_<timing::FanoutDesign<PC059IONode, PDIMasterNode>, uhal::Node>(m, "FanoutDesign<PC059IONode,PDIMasterNode>")
    .def("read_firmware_version", &timing::FanoutDesign<PC059IONode, PDIMasterNode>::read_firmware_version)
    .def("validate_firmware_version", &timing::FanoutDesign<PC059IONode, PDIMasterNode>::validate_firmware_version)
    .def("sync_timestamp", &timing::FanoutDesign<PC059IONode, PDIMasterNode>::sync_timestamp)
    .def("enable_fake_trigger",
         &timing::FanoutDesign<PC059IONode, PDIMasterNode>::enable_fake_trigger,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("switch_sfp_mux_channel", &timing::FanoutDesign<PC059IONode, PDIMasterNode>::switch_sfp_mux_channel)
    .def("apply_endpoint_delay", &timing::FanoutDesign<PC059IONode, PDIMasterNode>::apply_endpoint_delay)
    .def("measure_endpoint_rtt", &timing::FanoutDesign<PC059IONode, PDIMasterNode>::measure_endpoint_rtt)
    .def("scan_sfp_mux", &timing::FanoutDesign<PC059IONode, PDIMasterNode>::scan_sfp_mux);

  // PD-I fanout design on fib
  py::class_<timing::FanoutDesign<FIBIONode, PDIMasterNode>, uhal::Node>(m, "FanoutDesign<FIBIONode,PDIMasterNode>")
    .def("read_firmware_version", &timing::FanoutDesign<FIBIONode, PDIMasterNode>::read_firmware_version)
    .def("validate_firmware_version", &timing::FanoutDesign<FIBIONode, PDIMasterNode>::validate_firmware_version)
    .def("sync_timestamp", &timing::FanoutDesign<FIBIONode, PDIMasterNode>::sync_timestamp)
    .def("enable_fake_trigger",
         &timing::FanoutDesign<FIBIONode, PDIMasterNode>::enable_fake_trigger,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("switch_sfp_mux_channel", &timing::FanoutDesign<FIBIONode, PDIMasterNode>::switch_sfp_mux_channel)
    .def("apply_endpoint_delay", &timing::FanoutDesign<FIBIONode, PDIMasterNode>::apply_endpoint_delay)
    .def("measure_endpoint_rtt", &timing::FanoutDesign<FIBIONode, PDIMasterNode>::measure_endpoint_rtt)
    .def("scan_sfp_mux", &timing::FanoutDesign<FIBIONode, PDIMasterNode>::scan_sfp_mux);

  // PD-I ouroboros design on pc059
  py::class_<timing::OuroborosMuxDesign<PC059IONode>, uhal::Node>(
    m, "OuroborosMuxDesign<PC059IONode>")
    .def("read_firmware_version", &timing::OuroborosMuxDesign<PC059IONode>::read_firmware_version)
    .def("validate_firmware_version", &timing::OuroborosMuxDesign<PC059IONode>::validate_firmware_version)
    .def("sync_timestamp", &timing::OuroborosMuxDesign<PC059IONode>::sync_timestamp)
    .def("enable_fake_trigger",
         &timing::OuroborosMuxDesign<PC059IONode>::enable_fake_trigger,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("switch_sfp_mux_channel", &timing::OuroborosMuxDesign<PC059IONode>::switch_sfp_mux_channel)
    .def("apply_endpoint_delay", &timing::OuroborosMuxDesign<PC059IONode>::apply_endpoint_delay)
    .def("measure_endpoint_rtt", &timing::OuroborosMuxDesign<PC059IONode>::measure_endpoint_rtt)
    .def("scan_sfp_mux", &timing::OuroborosMuxDesign<PC059IONode>::scan_sfp_mux);

  // PD-I ouroboros design on fib
  py::class_<timing::OuroborosMuxDesign<FIBIONode>, uhal::Node>(
    m, "OuroborosMuxDesign<FIBIONode>")
    .def("read_firmware_version", &timing::OuroborosMuxDesign<FIBIONode>::read_firmware_version)
    .def("validate_firmware_version", &timing::OuroborosMuxDesign<FIBIONode>::validate_firmware_version)
    .def("sync_timestamp", &timing::OuroborosMuxDesign<FIBIONode>::sync_timestamp)
    .def("enable_fake_trigger",
         &timing::OuroborosMuxDesign<FIBIONode>::enable_fake_trigger,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("switch_sfp_mux_channel", &timing::OuroborosMuxDesign<FIBIONode>::switch_sfp_mux_channel)
    .def("apply_endpoint_delay", &timing::OuroborosMuxDesign<FIBIONode>::apply_endpoint_delay)
    .def( "measure_endpoint_rtt", &timing::OuroborosMuxDesign<FIBIONode>::measure_endpoint_rtt)
    .def("scan_sfp_mux", &timing::OuroborosMuxDesign<FIBIONode>::scan_sfp_mux);

  // Ouroboros on FMC
  py::class_<timing::OuroborosDesign<FMCIONode>, uhal::Node>(m, "OuroborosDesign<FMCIONode>")
    .def("read_firmware_version", &timing::OuroborosDesign<FMCIONode>::read_firmware_version)
    .def("validate_firmware_version", &timing::OuroborosDesign<FMCIONode>::validate_firmware_version)
    .def("sync_timestamp", &timing::OuroborosDesign<FMCIONode>::sync_timestamp)
    .def("get_status", &timing::OuroborosDesign<FMCIONode>::get_status)
    .def("enable_fake_trigger",
         &timing::OuroborosDesign<FMCIONode>::enable_fake_trigger,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("apply_endpoint_delay", &timing::OuroborosDesign<FMCIONode>::apply_endpoint_delay)
    .def("measure_endpoint_rtt", &timing::OuroborosDesign<FMCIONode>::measure_endpoint_rtt);

  // Ouroboros on SIM
  py::class_<timing::OuroborosDesign<SIMIONode>, uhal::Node>(m, "OuroborosDesign<SIMIONode>")
    .def("read_firmware_version", &timing::OuroborosDesign<SIMIONode>::read_firmware_version)
    .def("validate_firmware_version", &timing::OuroborosDesign<SIMIONode>::validate_firmware_version)
    .def("sync_timestamp", &timing::OuroborosDesign<SIMIONode>::sync_timestamp)
    .def("get_status", &timing::OuroborosDesign<SIMIONode>::get_status)
    .def("enable_fake_trigger",
         &timing::OuroborosDesign<SIMIONode>::enable_fake_trigger,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("apply_endpoint_delay", &timing::OuroborosDesign<SIMIONode>::apply_endpoint_delay)
    .def("measure_endpoint_rtt", &timing::OuroborosDesign<SIMIONode>::measure_endpoint_rtt);

  // Endpoint on FMC
  py::class_<timing::EndpointDesign<FMCIONode>, uhal::Node>(m, "EndpointDesign<FMCIONode>")
    .def("read_firmware_version", &timing::EndpointDesign<FMCIONode>::read_firmware_version)
    .def("validate_firmware_version", &timing::EndpointDesign<FMCIONode>::validate_firmware_version)
    .def("get_status", &timing::EndpointDesign<FMCIONode>::get_status)
    .def("get_endpoint_node", &timing::EndpointDesign<FMCIONode>::get_endpoint_node);

  // Chronos on FMC
  py::class_<timing::ChronosDesign<FMCIONode>, uhal::Node>(m, "ChronosDesign<FMCIONode>")
    .def("read_firmware_version", &timing::ChronosDesign<FMCIONode>::read_firmware_version)
    .def("validate_firmware_version", &timing::ChronosDesign<FMCIONode>::validate_firmware_version)
    .def("get_status", &timing::ChronosDesign<FMCIONode>::get_status)
    .def("get_hsi_node", &timing::ChronosDesign<FMCIONode>::get_hsi_node)
    .def("configure_hsi", 
         &timing::ChronosDesign<FMCIONode>::configure_hsi,
         py::arg("src"),
         py::arg("re_mask"),
         py::arg("fe_mask"),
         py::arg("inv_mask"),
         py::arg("rate"),
         py::arg("dispatch") = true);
    
  // CRT on FMC
  py::class_<timing::CRTDesign<FMCIONode>, uhal::Node>(m, "CRTDesign<FMCIONode>")
    .def("read_firmware_version", &timing::CRTDesign<FMCIONode>::read_firmware_version)
    .def("validate_firmware_version", &timing::CRTDesign<FMCIONode>::validate_firmware_version)
    .def("get_status", &timing::CRTDesign<FMCIONode>::get_status)
    .def("get_crt_node", &timing::CRTDesign<FMCIONode>::get_crt_node);
} // NOLINT

} // namespace python
} // namespace timing
} // namespace dunedaq