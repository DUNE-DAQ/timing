#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "pdt/PDIMasterNode.hpp"

namespace py = pybind11;

namespace dunedaq {
namespace pdt {
namespace python {

void
register_master(py::module& m) {
      py::class_<pdt::PDIMasterNode, uhal::Node> (m, "PDIMasterNode")
      .def(py::init<const uhal::Node&>())
      .def<void (pdt::PDIMasterNode::*)(uint32_t,uint32_t,uint32_t,uint32_t,bool,bool) const>("applyEndpointDelay", &pdt::PDIMasterNode::applyEndpointDelay, 
            py::arg("aAddr"), py::arg("aCDel"), py::arg("aFDel"), py::arg("aPDel"), py::arg("aControlSFP") = true, py::arg("aControlSFP") = true)
      .def("measureEndpointRTT", &pdt::PDIMasterNode::measureEndpointRTT, py::arg("aAddr"), py::arg("aControlSFP") = true)
      .def("switchEndpointSFP", &pdt::PDIMasterNode::switchEndpointSFP)
      .def("enableUpstreamEndpoint", &pdt::PDIMasterNode::enableUpstreamEndpoint)
      .def("sendFLCmd", &pdt::PDIMasterNode::sendFLCmd, py::arg("aCmd"), py::arg("aChan"), py::arg("aNumber") = 1)
      .def("enableFakeTrigger", &pdt::PDIMasterNode::enableFakeTrigger, py::arg("aChan"), py::arg("aRate"), py::arg("aPoisson") = false)
      .def("disableFakeTrigger", &pdt::PDIMasterNode::disableFakeTrigger)
      .def("enableSpillInterface", &pdt::PDIMasterNode::enableSpillInterface)
      .def("enableFakeSpills", &pdt::PDIMasterNode::enableFakeSpills, py::arg("aCycLen") = 16, py::arg("aSpillLen") = 8)
      .def("get_status", &pdt::PDIMasterNode::get_status, py::arg("aPrint") = false)
      .def("syncTimestamp", &pdt::PDIMasterNode::syncTimestamp)
      ;

      py::class_<pdt::TriggerReceiverNode, uhal::Node> (m, "TriggerReceiverNode")
      .def(py::init<const uhal::Node&>())
      .def("enable", &pdt::TriggerReceiverNode::enable)
      .def("disable", &pdt::TriggerReceiverNode::disable)
      .def("reset", &pdt::TriggerReceiverNode::reset)
      .def("enableTriggers", &pdt::TriggerReceiverNode::enableTriggers)
      .def("disableTriggers", &pdt::TriggerReceiverNode::disableTriggers)
      .def("get_status", &pdt::TriggerReceiverNode::get_status, py::arg("aPrint") = false)
      ;
}

} // namespace python
} // namespace pdt
} // namespace dunedaq