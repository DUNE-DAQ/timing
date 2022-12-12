/**
 * @file PDIMasterGlobalNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/PDIMasterGlobalNode.hpp"

#include "logging/Logging.hpp"
#include "timing/toolbox.hpp"

#include <string>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(PDIMasterGlobalNode)

//-----------------------------------------------------------------------------
PDIMasterGlobalNode::PDIMasterGlobalNode(const uhal::Node& node)
  : TimingNode(node)
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
PDIMasterGlobalNode::~PDIMasterGlobalNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
PDIMasterGlobalNode::in_spill() const
{
  return false;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
PDIMasterGlobalNode::tx_error() const
{
  return false;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
PDIMasterGlobalNode::readTimeStamp() const
{
  return 0;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
PDIMasterGlobalNode::read_spill_counter() const
{
  return 0;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterGlobalNode::select_partition() const
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterGlobalNode::lock_partition() const
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
PDIMasterGlobalNode::get_status(bool print_out) const
{
  std::stringstream status;
  auto subnodes = read_sub_nodes(getNode("csr.stat"));
  status << format_reg_table(subnodes, "Global state");
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterGlobalNode::enable_upstream_endpoint(uint32_t timeout) // NOLINT(build/unsigned)
{
  getNode("csr.ctrl.ep_en").write(0x0);
  getClient().dispatch();
  getNode("csr.ctrl.ep_en").write(0x1);
  getClient().dispatch();

  TLOG() << "Upstream endpoint reset, waiting for lock";

  auto start = std::chrono::high_resolution_clock::now();

  std::chrono::milliseconds ms_since_start(0);

  uhal::ValWord<uint32_t> ept_state; // NOLINT(build/unsigned)
  uhal::ValWord<uint32_t> ept_ready; // NOLINT(build/unsigned)

  // Wait for the endpoint to be happy
  while (ms_since_start.count() < timeout) {
    auto now = std::chrono::high_resolution_clock::now();
    ms_since_start = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

    millisleep(50);

    ept_state = getNode("csr.stat.ep_stat").read();
    ept_ready = getNode("csr.stat.ep_rdy").read();

    auto ept_edge = getNode("csr.stat.ep_edge").read();
    auto ept_fdel = getNode("csr.stat.ep_fdel").read();

    getClient().dispatch();

    TLOG_DEBUG(1) << "ept state: 0x" << std::hex << ept_state.value() << " ept ready: 0x" << ept_ready.value()
                  << " ept edge: 0x" << ept_edge.value() << " ept fdel: 0x" << ept_fdel.value();

    if (ept_ready.value() && ept_state.value() == 0x8) {
      TLOG_DEBUG(1) << "Endpoint locked: state= " << format_reg_value(ept_state);
      return;
    }
  }

  if (!ept_ready.value() || ept_state.value() != 0x8) {
    throw EndpointNotReady(ERS_HERE, "Master upstream", ept_state.value());
  } else {
    TLOG_DEBUG(1) << "Endpoint locked: state= " << format_reg_value(ept_state);
  }
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq