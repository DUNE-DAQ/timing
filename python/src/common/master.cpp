// Boost Headers
#include <boost/python/def.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/class.hpp>
#include <boost/python/enum.hpp>
#include <boost/python/copy_const_reference.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/operators.hpp>


#include "pdt/PDIMasterNode.hpp"

// Namespace resolution
using namespace boost::python;

// PDIMasterNode
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_PDIMasterNode_applyEndpointDelay_overloads, applyEndpointDelay, 4, 5);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_PDIMasterNode_measureEndpointRTT_overloads, measureEndpointRTT, 1, 2);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_PDIMasterNode_sendFLCmd_overloads, sendFLCmd, 2, 3);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_PDIMasterNode_enableFakeTrigger_overloads, enableFakeTrigger, 2, 3);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_PDIMasterNode_enableFakeSpills_overloads, enableFakeSpills, 0, 2);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_PDIMasterNode_getStatus_overloads, getStatus, 0, 1);

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_TriggerReceiverNode_getStatus_overloads, getStatus, 0, 1);


namespace pdt {
namespace python {

void
register_master() {
      class_<pdt::PDIMasterNode, bases<uhal::Node> > ("PDIMasterNode", init<const uhal::Node&>())
      .def<void (pdt::PDIMasterNode::*)(uint32_t,uint32_t,uint32_t,uint32_t,bool,bool) const>("applyEndpointDelay", &pdt::PDIMasterNode::applyEndpointDelay, pdt_PDIMasterNode_applyEndpointDelay_overloads())
      .def("measureEndpointRTT", &pdt::PDIMasterNode::measureEndpointRTT, pdt_PDIMasterNode_measureEndpointRTT_overloads())
      .def("switchEndpointSFP", &pdt::PDIMasterNode::switchEndpointSFP)
      .def("enableUpstreamEndpoint", &pdt::PDIMasterNode::enableUpstreamEndpoint)
      .def("sendFLCmd", &pdt::PDIMasterNode::sendFLCmd, pdt_PDIMasterNode_sendFLCmd_overloads())
      .def("enableFakeTrigger", &pdt::PDIMasterNode::enableFakeTrigger, pdt_PDIMasterNode_enableFakeTrigger_overloads())
      .def("disableFakeTrigger", &pdt::PDIMasterNode::disableFakeTrigger)
      .def("enableSpillInterface", &pdt::PDIMasterNode::enableSpillInterface)
      .def("enableFakeSpills", &pdt::PDIMasterNode::enableFakeSpills, pdt_PDIMasterNode_enableFakeSpills_overloads())
      .def("getStatus", &pdt::PDIMasterNode::getStatus, pdt_PDIMasterNode_getStatus_overloads())
      .def("syncTimestamp", &pdt::PDIMasterNode::syncTimestamp)
      ;

      class_<pdt::TriggerReceiverNode, bases<uhal::Node> > ("TriggerReceiverNode", init<const uhal::Node&>())
      .def("enable", &pdt::TriggerReceiverNode::enable)
      .def("disable", &pdt::TriggerReceiverNode::disable)
      .def("reset", &pdt::TriggerReceiverNode::reset)
      .def("enableTriggers", &pdt::TriggerReceiverNode::enableTriggers)
      .def("disableTriggers", &pdt::TriggerReceiverNode::disableTriggers)
      .def("getStatus", &pdt::TriggerReceiverNode::getStatus, pdt_TriggerReceiverNode_getStatus_overloads())
      ;
}

} // namespace python
} // namespace pdt