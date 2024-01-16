/**
 * @file EchoMonitorNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/UpstreamCDRNode.hpp"

#include "timing/TimingIssues.hpp"
#include "timing/toolbox.hpp"
#include "logging/Logging.hpp"

#include <string>
#include <chrono>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(UpstreamCDRNode)

//-----------------------------------------------------------------------------
UpstreamCDRNode::UpstreamCDRNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
UpstreamCDRNode::~UpstreamCDRNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
UpstreamCDRNode::get_status(bool print_out) const
{
  std::stringstream status;
  auto subnodes = read_sub_nodes(getNode("stat"));
  status << format_reg_table(subnodes, "Upstream CDR state");
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
UpstreamCDRNode::resync() const
{
  getNode("ctrl.resync").write(0x1);
  getNode("ctrl.resync").write(0x0);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq