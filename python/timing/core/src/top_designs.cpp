#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "timing/TLUIONode.hpp"
#include "timing/PC059IONode.hpp"
#include "timing/FMCIONode.hpp"

#include "timing/PDIMasterNode.hpp"

#include "timing/OverlordDesign.hpp"
#include "timing/FanoutDesign.hpp"
#include "timing/MasterMuxDesign.hpp"
#include "timing/BoreasDesign.hpp"

namespace py = pybind11;

namespace dunedaq {
namespace timing {
namespace python {

void
register_top_designs(py::module& m) {

      // Overlord on TLU
      py::class_<timing::OverlordDesign<TLUIONode>, uhal::Node> (m, "OverlordDesign<TLUIONode>")
      .def("get_status", &timing::OverlordDesign<TLUIONode>::get_status)
      .def("apply_endpoint_delay", &timing::OverlordDesign<TLUIONode>::apply_endpoint_delay)
      .def("measure_endpoint_rtt", &timing::OverlordDesign<TLUIONode>::measure_endpoint_rtt)
      ;

      // Overlord on FMC
      py::class_<timing::OverlordDesign<FMCIONode>, uhal::Node> (m, "OverlordDesign<FMCIONode>")
      .def("get_status", &timing::OverlordDesign<FMCIONode>::get_status)
      .def("apply_endpoint_delay", &timing::OverlordDesign<FMCIONode>::apply_endpoint_delay)
      .def("measure_endpoint_rtt", &timing::OverlordDesign<FMCIONode>::measure_endpoint_rtt)
      ;

      // Boreas on FMC
      py::class_<timing::BoreasDesign<FMCIONode>, uhal::Node> (m, "BoreasDesign<FMCIONode>")
      .def("get_status", &timing::BoreasDesign<FMCIONode>::get_status)
      .def("apply_endpoint_delay", &timing::BoreasDesign<FMCIONode>::apply_endpoint_delay)
      .def("measure_endpoint_rtt", &timing::BoreasDesign<FMCIONode>::measure_endpoint_rtt)
      ;

      // Boreas on TLU
      py::class_<timing::BoreasDesign<TLUIONode>, uhal::Node> (m, "BoreasDesign<TLUIONode>")
      .def("get_status", &timing::BoreasDesign<TLUIONode>::get_status)
      .def("apply_endpoint_delay", &timing::BoreasDesign<TLUIONode>::apply_endpoint_delay)
      .def("measure_endpoint_rtt", &timing::BoreasDesign<TLUIONode>::measure_endpoint_rtt)
      ;

      // Boreas on sim
      py::class_<timing::BoreasDesign<SIMIONode>, uhal::Node> (m, "BoreasDesign<SIMIONode>")
      .def("get_status", &timing::BoreasDesign<SIMIONode>::get_status)
      .def("apply_endpoint_delay", &timing::BoreasDesign<SIMIONode>::apply_endpoint_delay)
      .def("measure_endpoint_rtt", &timing::BoreasDesign<SIMIONode>::measure_endpoint_rtt)
      ;

      // PD-I fanout design
      py::class_<timing::FanoutDesign<PC059IONode,PDIMasterNode>, uhal::Node> (m, "FanoutDesign<PC059IONode,PDIMasterNode>")
      .def("switch_sfp_mux_channel", &timing::FanoutDesign<PC059IONode,PDIMasterNode>::switch_sfp_mux_channel)
      .def("apply_endpoint_delay", &timing::FanoutDesign<PC059IONode,PDIMasterNode>::apply_endpoint_delay)
      .def("measure_endpoint_rtt", &timing::FanoutDesign<PC059IONode,PDIMasterNode>::measure_endpoint_rtt)
      .def("scan_sfp_mux", &timing::FanoutDesign<PC059IONode,PDIMasterNode>::scan_sfp_mux)
      ;

      // PD-I ouroboros design on pc059
      py::class_<timing::MasterMuxDesign<PC059IONode,PDIMasterNode>, uhal::Node> (m, "MasterMuxDesign<PC059IONode,PDIMasterNode>")
      .def("switch_sfp_mux_channel", &timing::MasterMuxDesign<PC059IONode,PDIMasterNode>::switch_sfp_mux_channel)
      .def("apply_endpoint_delay", &timing::MasterMuxDesign<PC059IONode,PDIMasterNode>::apply_endpoint_delay)
      .def("measure_endpoint_rtt", &timing::MasterMuxDesign<PC059IONode,PDIMasterNode>::measure_endpoint_rtt)
      .def("scan_sfp_mux", &timing::MasterMuxDesign<PC059IONode,PDIMasterNode>::scan_sfp_mux)
      ;
}

} // namespace python
} // namespace timing
} // namespace dunedaq