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
      .def<void (pdt::PDIMasterNode::*)(uint32_t,uint32_t,uint32_t,uint32_t,bool,bool) const>("apply_endpoint_delay", &pdt::PDIMasterNode::apply_endpoint_delay, 
            py::arg("aAddr"), py::arg("aCDel"), py::arg("aFDel"), py::arg("aPDel"), py::arg("aControlSFP") = true, py::arg("aControlSFP") = true)
      .def("measureEndpointRTT", &pdt::PDIMasterNode::measureEndpointRTT, py::arg("aAddr"), py::arg("aControlSFP") = true)
      .def("switch_endpoint_sfp", &pdt::PDIMasterNode::switch_endpoint_sfp)
      .def("enable_upstream_endpoint", &pdt::PDIMasterNode::enable_upstream_endpoint)
      .def("send_fl_cmd", &pdt::PDIMasterNode::send_fl_cmd, py::arg("aCmd"), py::arg("aChan"), py::arg("aNumber") = 1)
      .def("enable_fake_trigger", &pdt::PDIMasterNode::enable_fake_trigger, py::arg("aChan"), py::arg("aRate"), py::arg("aPoisson") = false)
      .def("disable_fake_trigger", &pdt::PDIMasterNode::disable_fake_trigger)
      .def("enable_spill_interface", &pdt::PDIMasterNode::enable_spill_interface)
      .def("enable_fake_spills", &pdt::PDIMasterNode::enable_fake_spills, py::arg("aCycLen") = 16, py::arg("aSpillLen") = 8)
      .def("get_status", &pdt::PDIMasterNode::get_status, py::arg("aPrint") = false)
      .def("sync_timestamp", &pdt::PDIMasterNode::sync_timestamp)
      ;

      py::class_<pdt::TriggerReceiverNode, uhal::Node> (m, "TriggerReceiverNode")
      .def(py::init<const uhal::Node&>())
      .def("enable", &pdt::TriggerReceiverNode::enable)
      .def("disable", &pdt::TriggerReceiverNode::disable)
      .def("reset", &pdt::TriggerReceiverNode::reset)
      .def("enable_triggers", &pdt::TriggerReceiverNode::enable_triggers)
      .def("disable_triggers", &pdt::TriggerReceiverNode::disable_triggers)
      .def("get_status", &pdt::TriggerReceiverNode::get_status, py::arg("aPrint") = false)
      ;
}

} // namespace python
} // namespace pdt
} // namespace dunedaq