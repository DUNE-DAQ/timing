/**
 * @file SwitchyardNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/SwitchyardNode.hpp"

#include "logging/Logging.hpp"

#include <string>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(SwitchyardNode)

//-----------------------------------------------------------------------------
SwitchyardNode::SwitchyardNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
SwitchyardNode::~SwitchyardNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
SwitchyardNode::get_status(bool print_out) const
{
  std::stringstream status;
  auto subnodes = read_sub_nodes(getNode("csr.ctrl"));
  status << format_reg_table(subnodes, "Switchyard state");

  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SwitchyardNode::configure_master_source(uint8_t master_source, bool dispatch) const // NOLINT(build/unsigned)
{
  getNode("csr.ctrl.master_src").write(master_source);
  if (dispatch)
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SwitchyardNode::configure_endpoint_source(uint8_t endpoint_source, bool dispatch) const // NOLINT(build/unsigned)
{
  getNode("csr.ctrl.ep_src").write(endpoint_source);
  if (dispatch)
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq