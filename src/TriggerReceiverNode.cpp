/**
 * @file TriggerReceiverNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/TriggerReceiverNode.hpp"

#include "logging/Logging.hpp"

#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(TriggerReceiverNode)

//-----------------------------------------------------------------------------
TriggerReceiverNode::TriggerReceiverNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
TriggerReceiverNode::~TriggerReceiverNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
TriggerReceiverNode::get_status(bool print_out) const
{
  std::stringstream lStatus;

  auto lState = read_sub_nodes(getNode("csr.stat"), false);
  auto lControls = read_sub_nodes(getNode("csr.ctrl"), false);
  auto lCounters = getNode("ctrs").readBlock(0x10);
  getClient().dispatch();

  lStatus << format_reg_table(lState, "Trigger rx state");
  lStatus << format_reg_table(lControls, "Trigger rx controls");

  std::vector<uhal::ValVector<uint32_t>> lCountersContainer = { lCounters }; // NOLINT(build/unsigned)
  lStatus << format_counters_table(lCountersContainer, { "Counters" }, "Trig rx counters");

  if (print_out)
    TLOG() << lStatus.str();
  return lStatus.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
TriggerReceiverNode::enable() const
{
  getNode("csr.ctrl.ep_en").write(0x1);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
TriggerReceiverNode::disable() const
{
  getNode("csr.ctrl.ep_en").write(0x0);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
TriggerReceiverNode::reset() const
{
  getNode("csr.ctrl.ep_en").write(0x0);
  getNode("csr.ctrl.ep_en").write(0x1);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
TriggerReceiverNode::enable_triggers() const
{
  getNode("csr.ctrl.ext_trig_en").write(0x1);
  getClient().dispatch();
}
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
TriggerReceiverNode::disable_triggers() const
{
  getNode("csr.ctrl.ext_trig_en").write(0x0);
  getClient().dispatch();
}
//------------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq