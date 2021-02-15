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
      .def("applyEndpointDelay", &pdt::PDIMasterDesign<TLUIONode>::applyEndpointDelay)
      .def("measureEndpointRTT", &pdt::PDIMasterDesign<TLUIONode>::measureEndpointRTT)
      ;

      // PD-I master design on fmc
      py::class_<pdt::PDIMasterDesign<FMCIONode>, uhal::Node> (m, "PDIMasterDesign<FMCIONode>")
      .def("get_status", &pdt::PDIMasterDesign<FMCIONode>::get_status)
      .def("applyEndpointDelay", &pdt::PDIMasterDesign<FMCIONode>::applyEndpointDelay)
      .def("measureEndpointRTT", &pdt::PDIMasterDesign<FMCIONode>::measureEndpointRTT)
      ;

      // PD-I fanout design
      py::class_<pdt::FanoutDesign<PC059IONode,PDIMasterNode>, uhal::Node> (m, "FanoutDesign<PC059IONode,PDIMasterNode>")
      .def("switchSFPMUXChannel", &pdt::FanoutDesign<PC059IONode,PDIMasterNode>::switchSFPMUXChannel)
      .def("applyEndpointDelay", &pdt::FanoutDesign<PC059IONode,PDIMasterNode>::applyEndpointDelay)
      .def("measureEndpointRTT", &pdt::FanoutDesign<PC059IONode,PDIMasterNode>::measureEndpointRTT)
      .def("scanSFPMUX", &pdt::FanoutDesign<PC059IONode,PDIMasterNode>::scanSFPMUX)
      ;

      // PD-I ouroboros design on pc059
      py::class_<pdt::MasterMuxDesign<PC059IONode,PDIMasterNode>, uhal::Node> (m, "MasterMuxDesign<PC059IONode,PDIMasterNode>")
      .def("switchSFPMUXChannel", &pdt::MasterMuxDesign<PC059IONode,PDIMasterNode>::switchSFPMUXChannel)
      .def("applyEndpointDelay", &pdt::MasterMuxDesign<PC059IONode,PDIMasterNode>::applyEndpointDelay)
      .def("measureEndpointRTT", &pdt::MasterMuxDesign<PC059IONode,PDIMasterNode>::measureEndpointRTT)
      .def("scanSFPMUX", &pdt::MasterMuxDesign<PC059IONode,PDIMasterNode>::scanSFPMUX)
      ;
}

} // namespace python
} // namespace pdt
} // namespace dunedaq