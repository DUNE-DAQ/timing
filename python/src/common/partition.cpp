// Boost Headers
#include <boost/python/def.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/class.hpp>
#include <boost/python/enum.hpp>
#include <boost/python/copy_const_reference.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/operators.hpp>


#include "pdt/PartitionNode.hpp"

// Namespace resolution
using namespace boost::python;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_PartitionNode_configure_overloads, configure, 2, 3);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_PartitionNode_enable_overloads, enable, 0, 2);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_PartitionNode_enableTriggers_overloads, enableTriggers, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_PartitionNode_readEvents_overloads, readEvents, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_PartitionNode_start_overloads, start, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_PartitionNode_stop_overloads, stop, 0, 1);

namespace pdt {
namespace python {

void
register_partition() {
  class_<pdt::PartitionNode, bases<uhal::Node> > ("PartitionNode", init<const uhal::Node&>())
      .def("readTriggerMask", &pdt::PartitionNode::readTriggerMask)
      // .def("writeTriggerMask", &pdt::PartitionNode::writeTriggerMask)
      .def("configure", &pdt::PartitionNode::configure, pdt_PartitionNode_configure_overloads())
      .def("enableTriggers", &pdt::PartitionNode::enableTriggers, pdt_PartitionNode_enableTriggers_overloads())
      .def("readBufferWordCount", &pdt::PartitionNode::readBufferWordCount)
      .def("numEventsInBuffer", &pdt::PartitionNode::numEventsInBuffer)
      .def("readROBWarningOverflow", &pdt::PartitionNode::readROBWarningOverflow)
      .def("readROBError", &pdt::PartitionNode::readROBError)
      .def("readEvents", &pdt::PartitionNode::readEvents, pdt_PartitionNode_readEvents_overloads())
      .def("enable", &pdt::PartitionNode::enable, pdt_PartitionNode_enable_overloads())
      .def("reset", &pdt::PartitionNode::reset)
      .def("start", &pdt::PartitionNode::start, pdt_PartitionNode_start_overloads())
      .def("stop", &pdt::PartitionNode::stop, pdt_PartitionNode_stop_overloads())
      ;
}

} // namespace python
} // namespace pdt