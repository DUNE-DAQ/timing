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

namespace pdt {
namespace python {

void
register_partition() {
  class_<pdt::PartitionNode, bases<uhal::Node> > ("PartitionNode", init<const uhal::Node&>())
      .def("reset", &pdt::PartitionNode::reset)
      .def("start", &pdt::PartitionNode::start)
      .def("stop", &pdt::PartitionNode::stop)
      ;
}

} // namespace python
} // namespace pdt