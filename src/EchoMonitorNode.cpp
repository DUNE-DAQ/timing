/**
 * @file EchoMonitorNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/EchoMonitorNode.hpp"

#include "timing/TimingIssues.hpp"
#include "timing/toolbox.hpp"
#include "logging/Logging.hpp"

#include <string>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(EchoMonitorNode)

//-----------------------------------------------------------------------------
EchoMonitorNode::EchoMonitorNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
EchoMonitorNode::~EchoMonitorNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
EchoMonitorNode::get_status(bool print_out) const
{
  std::stringstream status;
  auto subnodes = read_sub_nodes(getNode("csr.stat"));
  status << format_reg_table(subnodes, "Echo mon state");
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint64_t // NOLINT(build/unsigned)
EchoMonitorNode::send_echo_and_measure_delay(int64_t timeout) const
{

  getNode("csr.ctrl.go").write(0x1);
  getClient().dispatch();

  auto start = std::chrono::high_resolution_clock::now();

  std::chrono::milliseconds ms_since_start(0);

  uhal::ValWord<uint32_t> done; // NOLINT(build/unsigned)

  while (ms_since_start.count() < timeout) {

    auto now = std::chrono::high_resolution_clock::now();
    ms_since_start = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

    millisleep(100);

    done = getNode("csr.stat.rx_done").read();
    getClient().dispatch();

    TLOG_DEBUG(0) << "rx done: " << std::hex << done.value();

    if (done.value())
      break;
  }

  if (!done.value()) {
    throw EchoTimeout(ERS_HERE, timeout);
  }

  auto delta_t = getNode("csr.stat.deltat").read();
  getClient().dispatch();

  TLOG_DEBUG(0) << "delta t: " << format_reg_value(delta_t.value(), 10);

  return delta_t.value();
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq