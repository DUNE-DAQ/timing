/**
 * @file DCSKMuxNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/DCSKMuxNode.hpp"

#include "timing/toolbox.hpp"

#include <string>
#include <math.h>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(DCSKMuxNode)

//-----------------------------------------------------------------------------
DCSKMuxNode::DCSKMuxNode(const uhal::Node& node)
  : MuxNodeInterface(node)
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
DCSKMuxNode::~DCSKMuxNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
DCSKMuxNode::get_status(bool print_out) const
{
  std::stringstream status;

  auto subnodes = read_sub_nodes(getNode("csr.stat"));
  status << format_reg_table(subnodes, "DCSK mux state");

  if (print_out)
    TLOG() << std::endl << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint8_t
DCSKMuxNode::read_active_mux() const // NOLINT(build/unsigned)
{
    auto active_sfp_mux_channel = getNode("csr.stat.sel").read();
    getClient().dispatch();
    return active_sfp_mux_channel.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
DCSKMuxNode::switch_mux(uint8_t mux_channel) const // NOLINT(build/unsigned)
{
    // TODO add mux channel validity check
    getNode("csr.ctrl.sel").write(mux_channel);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
DCSKMuxNode::set_mux_sel_source(uint8_t source) const // NOLINT(build/unsigned)
{
    // TODO add mux channel validity check
    getNode("csr.ctrl.sel_source").write(source);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------


} // namespace timing
} // namespace dunedaq
