/**
 * @file EndpointDesignInterface.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

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
EndpointDesignInterface::get_endpoint_node(uint32_t ept_id) const // NOLINT(build/unsigned)
{
  const std::string node_name = "endpoint" + std::to_string(ept_id);
  return uhal::Node::getNode<EndpointNode>(node_name);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
EndpointDesignInterface::get_number_of_endpoint_nodes() const
{
  std::string regex_string = "endpoint[0-9]+";
  return uhal::Node::getNodes(regex_string).size();
}
//-----------------------------------------------------------------------------

} // namespace dunedaq::timing