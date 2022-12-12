/**
 * @file OverlordDesignInterface.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/OverlordDesignInterface.hpp"

#include <sstream>
#include <string>

namespace dunedaq::timing {

//-----------------------------------------------------------------------------
OverlordDesignInterface::OverlordDesignInterface(const uhal::Node& node)
  : MasterDesignInterface(node)
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
OverlordDesignInterface::~OverlordDesignInterface() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
const TriggerReceiverNode&
OverlordDesignInterface::get_external_triggers_endpoint_node() const
{
  return uhal::Node::getNode<TriggerReceiverNode>("trig_rx");
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
OverlordDesignInterface::reset_external_triggers_endpoint() const
{
  this->get_external_triggers_endpoint_node().reset();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
OverlordDesignInterface::enable_external_triggers() const
{
  this->get_external_triggers_endpoint_node().enable_triggers();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
OverlordDesignInterface::disable_external_triggers() const
{
  this->get_external_triggers_endpoint_node().disable_triggers();
}
//-----------------------------------------------------------------------------
} // namespace dunedaq::timing