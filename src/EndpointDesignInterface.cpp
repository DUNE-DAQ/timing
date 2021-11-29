#include "timing/EndpointDesignInterface.hpp"

#include <sstream>
#include <string>

namespace dunedaq::timing {

//-----------------------------------------------------------------------------
EndpointDesignInterface::EndpointDesignInterface(const uhal::Node& node)
  : TopDesignInterface(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
EndpointDesignInterface::~EndpointDesignInterface()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
const EndpointNode&
EndpointDesignInterface::get_endpoint_node(uint32_t ept_id) const
{
  const std::string node_name = "endpoint" + std::to_string(ept_id);
  return uhal::Node::getNode<EndpointNode>(node_name);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t
EndpointDesignInterface::get_number_of_endpoint_nodes() const
{
  std::string regex_string = "endpoint[0-9]+";
  return uhal::Node::getNodes(regex_string).size();
}
//-----------------------------------------------------------------------------
}