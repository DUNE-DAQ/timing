#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "timing/TLUIONode.hpp"
#include "timing/PC059IONode.hpp"
#include "timing/FMCIONode.hpp"

#include "timing/PDIMasterNode.hpp"

#include "timing/PDIMasterDesign.hpp"
#include "timing/FanoutDesign.hpp"
#include "timing/MasterMuxDesign.hpp"

namespace py = pybind11;

namespace dunedaq {
namespace timing {
namespace python {

void
register_top_designs(py::module& m) {

      // PD-I master design
      py::class_<timing::PDIMasterDesign<TLUIONode>, uhal::Node> (m, "PDIMasterDesign<TLUIONode>")
      .def("get_status", &timing::PDIMasterDesign<TLUIONode>::get_status)
      .def("apply_endpoint_delay", &timing::PDIMasterDesign<TLUIONode>::apply_endpoint_delay)
      .def("measure_endpoint_rtt", &timing::PDIMasterDesign<TLUIONode>::measure_endpoint_rtt)
      ;

      // PD-I master design on fmc
      py::class_<timing::PDIMasterDesign<FMCIONode>, uhal::Node> (m, "PDIMasterDesign<FMCIONode>")
      .def("get_status", &timing::PDIMasterDesign<FMCIONode>::get_status)
      .def("apply_endpoint_delay", &timing::PDIMasterDesign<FMCIONode>::apply_endpoint_delay)
      .def("measure_endpoint_rtt", &timing::PDIMasterDesign<FMCIONode>::measure_endpoint_rtt)
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