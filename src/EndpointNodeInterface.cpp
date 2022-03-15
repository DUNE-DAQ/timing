/**
 * @file EndpointNodeInterface.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/EndpointNodeInterface.hpp"


#include <sstream>
#include <string>

namespace dunedaq::timing {

//-----------------------------------------------------------------------------
EndpointNodeInterface::EndpointNodeInterface(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
EndpointNodeInterface::~EndpointNodeInterface()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
EndpointNodeInterface::endpoint_ready() const
{
  auto ready_flag = getNode("csr.stat.ep_rdy").read();
  getClient().dispatch();
  return ready_flag.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t
EndpointNodeInterface::read_endpoint_state() const
{
  auto endpoint_state = getNode("csr.stat.ep_stat").read();
  getClient().dispatch();
  return endpoint_state.value();
}
//-----------------------------------------------------------------------------

} // namespace dunedaq::timing