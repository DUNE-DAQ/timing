/**
 * @file top_designs.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/BoreasDesign.hpp"
#include "timing/ChronosDesign.hpp"
#include "timing/CRTDesign.hpp"
#include "timing/FanoutDesign.hpp"
#include "timing/OuroborosDesign.hpp"
#include "timing/OuroborosMuxDesign.hpp"
#include "timing/OverlordDesign.hpp"
#include "timing/EndpointDesign.hpp"
#include "timing/KerberosDesign.hpp"
#include "timing/GaiaDesign.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace dunedaq {
namespace timing {
namespace python {

void
register_top_designs(py::module& m)
{

  // Overlord
  py::class_<timing::OverlordDesign, uhal::Node>(m, "OverlordDesign")
    .def("read_firmware_version", &timing::OverlordDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::OverlordDesign::validate_firmware_version)
    .def("sync_timestamp", &timing::OverlordDesign::sync_timestamp)
    .def("get_status", &timing::OverlordDesign::get_status)
    .def<void (timing::OverlordDesign::*)(uint32_t, double, bool) const>("enable_periodic_fl_cmd",
         &timing::OverlordDesign::enable_periodic_fl_cmd,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def<void (timing::OverlordDesign::*)(uint32_t, uint32_t, double, bool) const>("enable_periodic_fl_cmd",
         &timing::OverlordDesign::enable_periodic_fl_cmd,
         py::arg("command"),
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("apply_endpoint_delay", 
          &timing::OverlordDesign::apply_endpoint_delay,
          py::arg("address"),
          py::arg("coarse_delay"),
          py::arg("fine_delay"),
          py::arg("phase_delay"),
          py::arg("measure_rtt") = false,
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("measure_endpoint_rtt", 
          &timing::OverlordDesign::measure_endpoint_rtt,
          py::arg("address"),
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("get_external_triggers_endpoint_node",
         &timing::OverlordDesign::get_external_triggers_endpoint_node);

  // Boreas
  py::class_<timing::BoreasDesign, uhal::Node>(m, "BoreasDesign")
    .def("read_firmware_version", &timing::BoreasDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::BoreasDesign::validate_firmware_version)
    .def("sync_timestamp", &timing::BoreasDesign::sync_timestamp)
    .def("get_status", &timing::BoreasDesign::get_status)
    .def<void (timing::BoreasDesign::*)(uint32_t, double, bool) const>("enable_periodic_fl_cmd",
         &timing::BoreasDesign::enable_periodic_fl_cmd,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def<void (timing::BoreasDesign::*)(uint32_t, uint32_t, double, bool) const>("enable_periodic_fl_cmd",
         &timing::BoreasDesign::enable_periodic_fl_cmd,
         py::arg("command"),
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("apply_endpoint_delay", 
          &timing::BoreasDesign::apply_endpoint_delay,
          py::arg("address"),
          py::arg("coarse_delay"),
          py::arg("fine_delay"),
          py::arg("phase_delay"),
          py::arg("measure_rtt") = false,
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("measure_endpoint_rtt", 
          &timing::BoreasDesign::measure_endpoint_rtt,
          py::arg("address"),
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("get_hsi_node", &timing::BoreasDesign::get_hsi_node)
    .def("configure_hsi", 
         &timing::BoreasDesign::configure_hsi,
         py::arg("src"),
         py::arg("re_mask"),
         py::arg("fe_mask"),
         py::arg("inv_mask"),
         py::arg("rate"),
         py::arg("dispatch") = true);

  // Fanout
  py::class_<timing::FanoutDesign, uhal::Node>(m, "FanoutDesign")
    .def("read_firmware_version", &timing::FanoutDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::FanoutDesign::validate_firmware_version)
    .def("sync_timestamp", &timing::FanoutDesign::sync_timestamp)
    .def<void (timing::FanoutDesign::*)(uint32_t, double, bool) const>("enable_periodic_fl_cmd",
         &timing::FanoutDesign::enable_periodic_fl_cmd,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def<void (timing::FanoutDesign::*)(uint32_t, uint32_t, double, bool) const>("enable_periodic_fl_cmd",
         &timing::FanoutDesign::enable_periodic_fl_cmd,
         py::arg("command"),
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("switch_downstream_mux_channel", &timing::FanoutDesign::switch_downstream_mux_channel)
    .def("apply_endpoint_delay", 
          &timing::FanoutDesign::apply_endpoint_delay,
          py::arg("address"),
          py::arg("coarse_delay"),
          py::arg("fine_delay"),
          py::arg("phase_delay"),
          py::arg("measure_rtt") = false,
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("measure_endpoint_rtt", 
          &timing::FanoutDesign::measure_endpoint_rtt,
          py::arg("address"),
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("scan_sfp_mux", &timing::FanoutDesign::scan_sfp_mux);

  // Ouroboros mux
  py::class_<timing::OuroborosMuxDesign, uhal::Node>(m, "OuroborosMuxDesign")
    .def("read_firmware_version", &timing::OuroborosMuxDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::OuroborosMuxDesign::validate_firmware_version)
    .def("sync_timestamp", &timing::OuroborosMuxDesign::sync_timestamp)
    .def<void (timing::OuroborosMuxDesign::*)(uint32_t, double, bool) const>("enable_periodic_fl_cmd",
         &timing::OuroborosMuxDesign::enable_periodic_fl_cmd,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def<void (timing::OuroborosMuxDesign::*)(uint32_t, uint32_t, double, bool) const>("enable_periodic_fl_cmd",
         &timing::OuroborosMuxDesign::enable_periodic_fl_cmd,
         py::arg("command"),
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("switch_downstream_mux_channel", &timing::OuroborosMuxDesign::switch_downstream_mux_channel)
    .def("apply_endpoint_delay", 
          &timing::OuroborosMuxDesign::apply_endpoint_delay,
          py::arg("address"),
          py::arg("coarse_delay"),
          py::arg("fine_delay"),
          py::arg("phase_delay"),
          py::arg("measure_rtt") = false,
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("measure_endpoint_rtt", 
          &timing::OuroborosMuxDesign::measure_endpoint_rtt,
          py::arg("address"),
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("scan_sfp_mux", &timing::OuroborosMuxDesign::scan_sfp_mux);

  // Master mux
  py::class_<timing::MasterMuxDesign, uhal::Node>(m, "MasterMuxDesign")
    .def("read_firmware_version", &timing::MasterMuxDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::MasterMuxDesign::validate_firmware_version)
    .def("sync_timestamp", &timing::MasterMuxDesign::sync_timestamp)
    .def<void (timing::MasterMuxDesign::*)(uint32_t, double, bool) const>("enable_periodic_fl_cmd",
         &timing::MasterMuxDesign::enable_periodic_fl_cmd,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def<void (timing::MasterMuxDesign::*)(uint32_t, uint32_t, double, bool) const>("enable_periodic_fl_cmd",
         &timing::MasterMuxDesign::enable_periodic_fl_cmd,
         py::arg("command"),
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("switch_downstream_mux_channel", &timing::MasterMuxDesign::switch_downstream_mux_channel)
    .def("apply_endpoint_delay", 
          &timing::MasterMuxDesign::apply_endpoint_delay,
          py::arg("address"),
          py::arg("coarse_delay"),
          py::arg("fine_delay"),
          py::arg("phase_delay"),
          py::arg("measure_rtt") = false,
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("measure_endpoint_rtt", 
          &timing::MasterMuxDesign::measure_endpoint_rtt,
          py::arg("address"),
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("scan_sfp_mux", &timing::MasterMuxDesign::scan_sfp_mux);

  // Master
  py::class_<timing::MasterDesign, uhal::Node>(m, "MasterDesign")
    .def("read_firmware_version", &timing::MasterDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::MasterDesign::validate_firmware_version)
    .def("sync_timestamp", &timing::MasterDesign::sync_timestamp)
    .def("get_status", &timing::MasterDesign::get_status)
    .def<void (timing::MasterDesign::*)(uint32_t, double, bool) const>("enable_periodic_fl_cmd",
         &timing::MasterDesign::enable_periodic_fl_cmd,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def<void (timing::MasterDesign::*)(uint32_t, uint32_t, double, bool) const>("enable_periodic_fl_cmd",
         &timing::MasterDesign::enable_periodic_fl_cmd,
         py::arg("command"),
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("apply_endpoint_delay", 
          &timing::MasterDesign::apply_endpoint_delay,
          py::arg("address"),
          py::arg("coarse_delay"),
          py::arg("fine_delay"),
          py::arg("phase_delay"),
          py::arg("measure_rtt") = false,
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("measure_endpoint_rtt", 
          &timing::MasterDesign::measure_endpoint_rtt,
          py::arg("address"),
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1);

  // Ouroboros
  py::class_<timing::OuroborosDesign, uhal::Node>(m, "OuroborosDesign")
    .def("read_firmware_version", &timing::OuroborosDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::OuroborosDesign::validate_firmware_version)
    .def("sync_timestamp", &timing::OuroborosDesign::sync_timestamp)
    .def("get_status", &timing::OuroborosDesign::get_status)
    .def<void (timing::OuroborosDesign::*)(uint32_t, double, bool) const>("enable_periodic_fl_cmd",
         &timing::OuroborosDesign::enable_periodic_fl_cmd,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def<void (timing::OuroborosDesign::*)(uint32_t, uint32_t, double, bool) const>("enable_periodic_fl_cmd",
         &timing::OuroborosDesign::enable_periodic_fl_cmd,
         py::arg("command"),
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("apply_endpoint_delay", 
          &timing::OuroborosDesign::apply_endpoint_delay,
          py::arg("address"),
          py::arg("coarse_delay"),
          py::arg("fine_delay"),
          py::arg("phase_delay"),
          py::arg("measure_rtt") = false,
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("measure_endpoint_rtt", 
          &timing::OuroborosDesign::measure_endpoint_rtt,
          py::arg("address"),
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1);

  // Endpoint
  py::class_<timing::EndpointDesign, uhal::Node>(m, "EndpointDesign")
    .def("read_firmware_version", &timing::EndpointDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::EndpointDesign::validate_firmware_version)
    .def("get_status", &timing::EndpointDesign::get_status);

  // Chronos
  py::class_<timing::ChronosDesign, uhal::Node>(m, "ChronosDesign")
    .def("read_firmware_version", &timing::ChronosDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::ChronosDesign::validate_firmware_version)
    .def("get_status", &timing::ChronosDesign::get_status)
    .def("get_hsi_node", &timing::ChronosDesign::get_hsi_node)
    .def("configure_hsi", 
         &timing::ChronosDesign::configure_hsi,
         py::arg("src"),
         py::arg("re_mask"),
         py::arg("fe_mask"),
         py::arg("inv_mask"),
         py::arg("rate"),
         py::arg("dispatch") = true);
    
  // CRT 
  py::class_<timing::CRTDesign, uhal::Node>(m, "CRTDesign")
    .def("read_firmware_version", &timing::CRTDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::CRTDesign::validate_firmware_version)
    .def("get_status", &timing::CRTDesign::get_status)
    .def("get_crt_node", &timing::CRTDesign::get_crt_node);

  // Kerberos
  py::class_<timing::KerberosDesign, uhal::Node>(m, "KerberosDesign")
    .def("read_firmware_version", &timing::KerberosDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::KerberosDesign::validate_firmware_version)
    .def("sync_timestamp", &timing::KerberosDesign::sync_timestamp)
    .def<void (timing::KerberosDesign::*)(uint32_t, double, bool) const>("enable_periodic_fl_cmd",
         &timing::KerberosDesign::enable_periodic_fl_cmd,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def<void (timing::KerberosDesign::*)(uint32_t, uint32_t, double, bool) const>("enable_periodic_fl_cmd",
         &timing::KerberosDesign::enable_periodic_fl_cmd,
         py::arg("command"),
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
//    .def("switch_downstream_mux_channel", &timing::FanoutDesign::switch_downstream_mux_channel)
    .def("apply_endpoint_delay",
          &timing::KerberosDesign::apply_endpoint_delay,
          py::arg("address"),
          py::arg("coarse_delay"),
          py::arg("fine_delay"),
          py::arg("phase_delay"),
          py::arg("measure_rtt") = false,
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("measure_endpoint_rtt",
          &timing::KerberosDesign::measure_endpoint_rtt,
          py::arg("address"),
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    ;

  // Gaia
  py::class_<timing::GaiaDesign, uhal::Node>(m, "GaiaDesign")
    .def("read_firmware_version", &timing::GaiaDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::GaiaDesign::validate_firmware_version)
    .def("sync_timestamp", &timing::GaiaDesign::sync_timestamp)
    .def<void (timing::GaiaDesign::*)(uint32_t, double, bool) const>("enable_periodic_fl_cmd",
         &timing::GaiaDesign::enable_periodic_fl_cmd,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def<void (timing::GaiaDesign::*)(uint32_t, uint32_t, double, bool) const>("enable_periodic_fl_cmd",
         &timing::GaiaDesign::enable_periodic_fl_cmd,
         py::arg("command"),
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
//    .def("switch_downstream_mux_channel", &timing::FanoutDesign::switch_downstream_mux_channel)
    .def("apply_endpoint_delay",
          &timing::GaiaDesign::apply_endpoint_delay,
          py::arg("address"),
          py::arg("coarse_delay"),
          py::arg("fine_delay"),
          py::arg("phase_delay"),
          py::arg("measure_rtt") = false,
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("measure_endpoint_rtt",
          &timing::GaiaDesign::measure_endpoint_rtt,
          py::arg("address"),
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("switch_upstream_mux_channel", &timing::GaiaDesign::switch_upstream_mux_channel, py::arg("mux_channel"))
    .def("read_active_upstream_mux_channel", &timing::GaiaDesign::read_active_upstream_mux_channel);
    ;
} // NOLINT

} // namespace python
} // namespace timing
} // namespace dunedaq