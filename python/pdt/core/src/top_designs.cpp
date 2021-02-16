#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "pdt/TLUIONode.hpp"
#include "pdt/PC059IONode.hpp"
#include "pdt/FMCIONode.hpp"

#include "pdt/PDIMasterNode.hpp"

#include "pdt/PDIMasterDesign.hpp"
#include "pdt/FanoutDesign.hpp"
#include "pdt/MasterMuxDesign.hpp"

namespace py = pybind11;

namespace dunedaq {
namespace pdt {
namespace python {

void
register_top_designs(py::module& m) {

      // PD-I master design
      py::class_<pdt::PDIMasterDesign<TLUIONode>, uhal::Node> (m, "PDIMasterDesign<TLUIONode>")
      .def("get_status", &pdt::PDIMasterDesign<TLUIONode>::get_status)
      .def("apply_endpoint_delay", &pdt::PDIMasterDesign<TLUIONode>::apply_endpoint_delay)
      .def("measureEndpointRTT", &pdt::PDIMasterDesign<TLUIONode>::measureEndpointRTT)
      ;

      // PD-I master design on fmc
      py::class_<pdt::PDIMasterDesign<FMCIONode>, uhal::Node> (m, "PDIMasterDesign<FMCIONode>")
      .def("get_status", &pdt::PDIMasterDesign<FMCIONode>::get_status)
      .def("apply_endpoint_delay", &pdt::PDIMasterDesign<FMCIONode>::apply_endpoint_delay)
      .def("measureEndpointRTT", &pdt::PDIMasterDesign<FMCIONode>::measureEndpointRTT)
      ;

      // PD-I fanout design
      py::class_<pdt::FanoutDesign<PC059IONode,PDIMasterNode>, uhal::Node> (m, "FanoutDesign<PC059IONode,PDIMasterNode>")
      .def("switch_sfp_mux_channel", &pdt::FanoutDesign<PC059IONode,PDIMasterNode>::switch_sfp_mux_channel)
      .def("apply_endpoint_delay", &pdt::FanoutDesign<PC059IONode,PDIMasterNode>::apply_endpoint_delay)
      .def("measureEndpointRTT", &pdt::FanoutDesign<PC059IONode,PDIMasterNode>::measureEndpointRTT)
      .def("scan_sfp_mux", &pdt::FanoutDesign<PC059IONode,PDIMasterNode>::scan_sfp_mux)
      ;

      // PD-I ouroboros design on pc059
      py::class_<pdt::MasterMuxDesign<PC059IONode,PDIMasterNode>, uhal::Node> (m, "MasterMuxDesign<PC059IONode,PDIMasterNode>")
      .def("switch_sfp_mux_channel", &pdt::MasterMuxDesign<PC059IONode,PDIMasterNode>::switch_sfp_mux_channel)
      .def("apply_endpoint_delay", &pdt::MasterMuxDesign<PC059IONode,PDIMasterNode>::apply_endpoint_delay)
      .def("measureEndpointRTT", &pdt::MasterMuxDesign<PC059IONode,PDIMasterNode>::measureEndpointRTT)
      .def("scan_sfp_mux", &pdt::MasterMuxDesign<PC059IONode,PDIMasterNode>::scan_sfp_mux)
      ;
}

} // namespace python
} // namespace pdt
} // namespace dunedaq