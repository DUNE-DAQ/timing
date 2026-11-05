/**
 * @file top_designs.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/BoreasDesign.hpp"
#include "timing/ChronosDesign.hpp"
#include "timing/FanoutDesign.hpp"
#include "timing/OuroborosDesign.hpp"
#include "timing/OuroborosMuxDesign.hpp"
#include "timing/EndpointDesign.hpp"
#include "timing/KerberosDesign.hpp"
#include "timing/GaiaDesign.hpp"
#include "timing/HadesDesign.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace dunedaq {
namespace timing {
namespace python {

void
register_top_designs(py::module& m)
{
  // Master interface
  py::class_<timing::MasterDesignInterface, uhal::Node>(m, "MasterDesignInterface")
    .def("measure_endpoint_rtt",
      &timing::MasterDesignInterface::measure_endpoint_rtt,
      py::arg("address"),
      py::arg("control_sfp") = true
    )
  ;

  // Endpoint interface
  py::class_<timing::EndpointDesignInterface, uhal::Node>(m, "EndpointDesignInterface")
    .def("get_endpoint_node_plain", &timing::EndpointDesignInterface::get_endpoint_node_plain)
    .def("get_number_of_endpoint_nodes", &timing::EndpointDesignInterface::get_number_of_endpoint_nodes)
  ;

  // Endpoint
  py::class_<timing::EndpointDesign, timing::EndpointDesignInterface, uhal::Node>(m, "EndpointDesign", py::multiple_inheritance())
    .def("read_firmware_version", &timing::EndpointDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::EndpointDesign::validate_firmware_version)
    .def("get_status", &timing::EndpointDesign::get_status)
    .def("configure", &timing::EndpointDesign::configure, py::arg("clock_source"))
  ;

  // Master
  py::class_<timing::MasterDesign, timing::MasterDesignInterface, uhal::Node>(m, "MasterDesign", py::multiple_inheritance())
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
      py::arg("cycle_delay"),
      py::arg("phase")
      )
    .def("configure", &timing::MasterDesign::configure, py::arg("clock_source"), py::arg("ts_source"))
  ;

  // HSI interface
  py::class_<timing::HSIDesignInterface, timing::EndpointDesignInterface, uhal::Node>(m, "HSIDesignInterface")
    .def("get_hsi_node", &timing::HSIDesignInterface::get_hsi_node)
    .def("configure_hsi",
      &timing::ChronosDesign::configure_hsi,
      py::arg("src"),
      py::arg("re_mask"),
      py::arg("fe_mask"),
      py::arg("inv_mask"),
      py::arg("rate"),
      py::arg("dispatch") = true)
  ;

  // Boreas
  py::class_<timing::BoreasDesign, timing::MasterDesign,
  timing::MasterDesignInterface, timing::HSIDesignInterface,
  uhal::Node>(m, "BoreasDesign", py::multiple_inheritance())
    .def("get_status", &timing::BoreasDesign::get_status)
    .def("configure", &timing::BoreasDesign::configure, py::arg("clock_source"), py::arg("ts_source"))
  ;

  // Ouroboros
  py::class_<timing::OuroborosDesign, uhal::Node>(m, "OuroborosDesign")
    .def("get_status", &timing::OuroborosDesign::get_status)
    .def("configure", &timing::OuroborosDesign::configure, py::arg("clock_source"), py::arg("ts_source"))
  ;

  // Mux
  py::class_<timing::MasterMuxDesignInterface, uhal::Node>(m, "MasterMuxDesignInterface", py::multiple_inheritance())
    .def<uint32_t (timing::MasterMuxDesignInterface::*)(uint16_t, uint8_t, bool) const>("measure_endpoint_rtt",
      &timing::MasterMuxDesignInterface::measure_endpoint_rtt,
      py::arg("address"),
      py::arg("master_mux"),
      py::arg("control_sfp") = true
    )
    //.def("scan_sfp_mux", &timing::MasterMuxDesign::scan_sfp_mux)
  ;

  // Fanout
  py::class_<timing::FanoutDesign, uhal::Node>(m, "FanoutDesign")
    .def("read_firmware_version", &timing::FanoutDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::FanoutDesign::validate_firmware_version)
    .def("switch_mux", &timing::FanoutDesign::switch_mux, py::arg("mux"), py::arg("resync_cdr") = false)
    .def("read_active_mux", &timing::FanoutDesign::read_active_mux)
    .def("configure", &timing::FanoutDesign::configure, py::arg("clock_source"))
    ;

  // Master mux
  py::class_<timing::MasterMuxDesign, timing::MasterDesign, timing::MasterMuxDesignInterface, uhal::Node>(m, "MasterMuxDesign", py::multiple_inheritance())
    .def("switch_mux", &timing::MasterMuxDesign::switch_mux, py::arg("mux"), py::arg("resync_cdr") = false)
    .def("read_active_mux", &timing::MasterMuxDesign::read_active_mux)
    .def("configure", &timing::MasterMuxDesign::configure, py::arg("clock_source"), py::arg("ts_source"))
    ;

  // Ouroboros mux
  py::class_<timing::OuroborosMuxDesign, timing::MasterMuxDesign, timing::MasterDesign, timing::MasterMuxDesignInterface, uhal::Node>(m, "OuroborosMuxDesign", py::multiple_inheritance())
    .def("switch_mux", &timing::OuroborosMuxDesign::switch_mux, py::arg("mux"), py::arg("resync_cdr") = false)
    .def("read_active_mux", &timing::OuroborosMuxDesign::read_active_mux)
    .def("configure", &timing::OuroborosMuxDesign::configure, py::arg("clock_source"), py::arg("ts_source"))
    ;

  // Chronos
  py::class_<timing::ChronosDesign, timing::HSIDesignInterface, uhal::Node>(m, "ChronosDesign", py::multiple_inheritance())
    .def("read_firmware_version", &timing::ChronosDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::ChronosDesign::validate_firmware_version)
    .def("get_status", &timing::ChronosDesign::get_status)
    .def("configure", &timing::ChronosDesign::configure, py::arg("clock_source"))
  ;

  // Hades
  py::class_<timing::HadesDesign, timing::ChronosDesign, uhal::Node>(m, "HadesDesign", py::multiple_inheritance())
  ;

  // Kerberos
  py::class_<timing::KerberosDesign, timing::MasterDesign,
  timing::MasterMuxDesignInterface, timing::EndpointDesignInterface, uhal::Node>(m, "KerberosDesign", py::multiple_inheritance())
    .def("switch_mux", &timing::KerberosDesign::switch_mux, py::arg("mux"), py::arg("resync_cdr") = false)
    .def("read_active_mux", &timing::KerberosDesign::read_active_mux)
    .def("configure", &timing::KerberosDesign::configure, py::arg("clock_source"), py::arg("ts_source"))
    .def("switch_timing_source_mux", &timing::KerberosDesign::switch_timing_source_mux, py::arg("mux_channel"))
    .def("switch_timing_source", &timing::KerberosDesign::switch_timing_source, py::arg("clock_source"))
    .def("read_active_timing_source_mux", &timing::KerberosDesign::read_active_timing_source_mux)
  ;

  // Gaia
  py::class_<timing::GaiaDesign, timing::MasterDesign,
  timing::MasterMuxDesignInterface, timing::EndpointDesignInterface,
  uhal::Node>(m, "GaiaDesign", py::multiple_inheritance())
    .def("switch_mux", &timing::GaiaDesign::switch_mux, py::arg("mux"), py::arg("resync_cdr") = false)
    .def("read_active_mux", &timing::GaiaDesign::read_active_mux)
    .def<void (timing::GaiaDesign::*)(ClockSource, TimestampSource, TimestampTimebase) const>
    ("configure", &timing::GaiaDesign::configure,
      py::arg("clock_source"),
      py::arg("ts_source"),
      py::arg("timebase"))
  ;
} // NOLINT

} // namespace python
} // namespace timing
} // namespace dunedaq