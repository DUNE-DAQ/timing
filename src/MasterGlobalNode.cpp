/**
 * @file MasterGlobalNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/MasterGlobalNode.hpp"

#include "logging/Logging.hpp"

#include <string>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(MasterGlobalNode)

//-----------------------------------------------------------------------------
MasterGlobalNode::MasterGlobalNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
MasterGlobalNode::~MasterGlobalNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
MasterGlobalNode::get_status(bool print_out) const
{
  std::stringstream status;
  auto ctrl = read_sub_nodes(getNode("csr.ctrl"));
  status << format_reg_table(ctrl, "Master global controls");
  
  auto stat = read_sub_nodes(getNode("csr.stat"));
  status << format_reg_table(stat, "Master global state");
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MasterGlobalNode::enable_upstream_endpoint(uint32_t timeout) const // NOLINT(build/unsigned)
{
  getNode("csr.ctrl.resync_cdr").write(0x1);
  getClient().dispatch();
  getNode("csr.ctrl.resync_cdr").write(0x0);
  getClient().dispatch();

  TLOG() << "Upstream CDR reset, waiting for lock";

  auto start = std::chrono::high_resolution_clock::now();

  std::chrono::milliseconds ms_since_start(0);

  uhal::ValWord<uint32_t> rx_ready;  // NOLINT(build/unsigned)
  uhal::ValWord<uint32_t> cdr_ready;  // NOLINT(build/unsigned)

  // Wait for the endpoint to be happy
  while (ms_since_start.count() < timeout) {
    auto now = std::chrono::high_resolution_clock::now();
    ms_since_start = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

    millisleep(10);

    rx_ready = getNode("csr.stat.rx_rdy").read();
    cdr_ready = getNode("csr.stat.cdr_locked").read();
    getClient().dispatch();

    TLOG_DEBUG(1) << "ept ready: 0x" << rx_ready.value();

    if (rx_ready.value() && cdr_ready.value()) {
      TLOG_DEBUG(1) << "Master endpoint and CDR ready!";
      return;
    }
  }

  if (!rx_ready.value() || !cdr_ready.value()) {
    throw EndpointNotReady(ERS_HERE, "Master upstream", rx_ready.value());
  } else {
    TLOG_DEBUG(1) << "Master endpoint ready";
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
MasterGlobalNode::read_upstream_endpoint_ready() const
{
  auto rx_ready = getNode("csr.stat.rx_rdy").read();
  getClient().dispatch();
  return rx_ready.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MasterGlobalNode::reset_command_counters(uint32_t timeout) const // NOLINT(build/unsigned)
{
  getNode("csr.ctrl.clr_ctrs").write(0x1);
  getClient().dispatch();

  TLOG_DEBUG(1) << "Command counters reset, waiting for them to be ready";

  auto start = std::chrono::high_resolution_clock::now();

  std::chrono::milliseconds ms_since_start(0);

  uhal::ValWord<uint32_t> counters_ready;  // NOLINT(build/unsigned)

  // Wait for the endpoint to be happy
  while (ms_since_start.count() < timeout) {
    auto now = std::chrono::high_resolution_clock::now();
    ms_since_start = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

    millisleep(10);

    counters_ready = getNode("csr.stat.ctrs_rdy").read();
    getClient().dispatch();

    TLOG_DEBUG(1) << "counters ready: 0x" << counters_ready.value();

    if (counters_ready.value()) {
      TLOG_DEBUG(1) << "Master command counters ready!";
      return;
    }
  }

  if (!counters_ready.value()) {
    // TODO throw something
    TLOG() << "Command counters did not become ready";
  } else {
    TLOG_DEBUG(1) << "Command counters ready";
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
MasterGlobalNode::read_counters_ready() const
{
  auto counters_ready = getNode("csr.stat.ctrs_rdy").read();
  getClient().dispatch();
  return counters_ready.value();
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq