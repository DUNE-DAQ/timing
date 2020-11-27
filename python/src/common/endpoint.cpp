// Boost Headers
#include <boost/python/def.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/class.hpp>
#include <boost/python/enum.hpp>
#include <boost/python/copy_const_reference.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/operators.hpp>


#include "pdt/EndpointNode.hpp"
#include "pdt/CRTNode.hpp"

// Namespace resolution
using namespace boost::python;


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_EndpointNode_enable_overloads, enable, 0, 2);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_EndpointNode_reset_overloads, reset, 0, 2);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_EndpointNode_readDataBuffer_overloads, readDataBuffer, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_EndpointNode_getDataBufferTable_overloads, getDataBufferTable, 0, 2);

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_CRTNode_getStatus_overloads, getStatus, 0, 1);

namespace pdt {
namespace python {

void
register_endpoint() {
  class_<pdt::EndpointNode, bases<uhal::Node> > ("EndpointNode", init<const uhal::Node&>())
      .def("disable", &pdt::EndpointNode::disable)
      .def("enable", &pdt::EndpointNode::enable, pdt_EndpointNode_enable_overloads())
      .def("reset", &pdt::EndpointNode::reset, pdt_EndpointNode_reset_overloads())
      .def("readBufferCount", &pdt::EndpointNode::readBufferCount)
      .def("readDataBuffer", &pdt::EndpointNode::readDataBuffer, pdt_EndpointNode_readDataBuffer_overloads())
      .def("getDataBufferTable", &pdt::EndpointNode::getDataBufferTable, pdt_EndpointNode_getDataBufferTable_overloads())
      .def("readVersion", &pdt::EndpointNode::readVersion)
      .def("readTimestamp", &pdt::EndpointNode::readTimestamp)
      .def("readClockFrequency", &pdt::EndpointNode::readClockFrequency)
      ;

  class_<pdt::CRTNode, bases<uhal::Node> > ("CRTNode", init<const uhal::Node&>())
      .def("disable", &pdt::CRTNode::disable)
      .def("enable", &pdt::CRTNode::enable)
      .def("getStatus", &pdt::CRTNode::getStatus, pdt_CRTNode_getStatus_overloads())
      .def("readLastPulseTimestamp", &pdt::CRTNode::readLastPulseTimestamp)
      ;
}

} // namespace python
} // namespace pdt