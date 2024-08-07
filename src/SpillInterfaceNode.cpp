/**
 * @file SpillInterfaceNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/SpillInterfaceNode.hpp"

#include "timing/toolbox.hpp"
#include "logging/Logging.hpp"

#include <string>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(SpillInterfaceNode)

//-----------------------------------------------------------------------------
SpillInterfaceNode::SpillInterfaceNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
SpillInterfaceNode::~SpillInterfaceNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
SpillInterfaceNode::get_status(bool print_out) const
{
  std::stringstream status;
  auto subnodes = read_sub_nodes(getNode("csr.stat"));
  status << format_reg_table(subnodes, "Spill interface state");

  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SpillInterfaceNode::enable() const
{
  getNode("csr.ctrl.src").write(0);
  getNode("csr.ctrl.en").write(1);
  getClient().dispatch();
  TLOG() << "Spill interface enabled";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SpillInterfaceNode::disable() const
{
  getNode("csr.ctrl.en").write(0);
  getClient().dispatch();
  TLOG() << "Spill interface disabled";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SpillInterfaceNode::enable_fake_spills(uint32_t cycle_length, uint32_t spill_length) const // NOLINT(build/unsigned)
{

  // Enables the internal spill generator.
  // Configures the internal command generator to produce spills at a defined frequency and length

  // Rate = 50 Mhz / 2**( 12 + divider ) for divider between 0 and 15
  // cyc_len and spill_len are in units of 1 / (50MHz / 2^24) = 0.34s

  getNode("csr.ctrl.fake_cyc_len").write(cycle_length);
  getNode("csr.ctrl.fake_spill_len").write(spill_length);
  getNode("csr.ctrl.src").write(1);
  getNode("csr.ctrl.en").write(1);
  getClient().dispatch();
  TLOG() << "Fake spills enabled";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
SpillInterfaceNode::read_in_spill() const
{
  auto in_spill = getNode("csr.stat.in_spill").read();
  getClient().dispatch();
  return in_spill.value();
}
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// void
// SpillInterfaceNode::get_info(timingfirmwareinfo::PDISpillInterfaceMonitorData& mon_data) const
// {
//   auto ctrl_nodes = read_sub_nodes(getNode("csr.ctrl"));
//   auto stat_nodes = read_sub_nodes(getNode("csr.stat"));

//   mon_data.spill_interface_enabled = ctrl_nodes["en"];
//   mon_data.source = ctrl_nodes["src"];
//   mon_data.in_spill = stat_nodes["in_spill"];
// }
// //-----------------------------------------------------------------------------

// //-----------------------------------------------------------------------------
// void
// SpillInterfaceNode::get_info(opmonlib::InfoCollector& ic, int /*level*/) const
// {
//   timingfirmwareinfo::PDISpillInterfaceMonitorData mon_data;
//   this->get_info(mon_data);
//   ic.add(mon_data);
// }
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq
