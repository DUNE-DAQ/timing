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
            py::arg("address"), py::arg("coarse_delay"), py::arg("fine_delay"), py::arg("phase_delay"), py::arg("control_sfp") = true, py::arg("control_sfp") = true)
      .def("measure_endpoint_rtt", &pdt::PDIMasterNode::measure_endpoint_rtt, py::arg("address"), py::arg("control_sfp") = true)
      .def("switch_endpoint_sfp", &pdt::PDIMasterNode::switch_endpoint_sfp)
      .def("enable_upstream_endpoint", &pdt::PDIMasterNode::enable_upstream_endpoint)
      .def("send_fl_cmd", &pdt::PDIMasterNode::send_fl_cmd, py::arg("command"), py::arg("channel"), py::arg("number_of_commands") = 1)
      .def("enable_fake_trigger", &pdt::PDIMasterNode::enable_fake_trigger, py::arg("channel"), py::arg("rate"), py::arg("poisson") = false)
      .def("disable_fake_trigger", &pdt::PDIMasterNode::disable_fake_trigger)
      .def("enable_spill_interface", &pdt::PDIMasterNode::enable_spill_interface)
      .def("enable_fake_spills", &pdt::PDIMasterNode::enable_fake_spills, py::arg("cycle_length") = 16, py::arg("spill_length") = 8)
      .def("get_status", &pdt::PDIMasterNode::get_status, py::arg("print_out") = false)
      .def("sync_timestamp", &pdt::PDIMasterNode::sync_timestamp)
      ;

      py::class_<pdt::TriggerReceiverNode, uhal::Node> (m, "TriggerReceiverNode")
      .def(py::init<const uhal::Node&>())
      .def("enable", &pdt::TriggerReceiverNode::enable)
      .def("disable", &pdt::TriggerReceiverNode::disable)
      .def("reset", &pdt::TriggerReceiverNode::reset)
      .def("enable_triggers", &pdt::TriggerReceiverNode::enable_triggers)
      .def("disable_triggers", &pdt::TriggerReceiverNode::disable_triggers)
      .def("get_status", &pdt::TriggerReceiverNode::get_status, py::arg("print_out") = false)
      ;
}

} // namespace python
} // namespace pdt
} // namespace dunedaq