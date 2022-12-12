/**
 * @file PDIEchoMonitorNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/PDIEchoMonitorNode.hpp"

#include "logging/Logging.hpp"
#include "timing/TimingIssues.hpp"
#include "timing/toolbox.hpp"

#include <string>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(PDIEchoMonitorNode)

//-----------------------------------------------------------------------------
PDIEchoMonitorNode::PDIEchoMonitorNode(const uhal::Node& node)
  : EchoMonitorNode(node)
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
PDIEchoMonitorNode::~PDIEchoMonitorNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint64_t // NOLINT(build/unsigned)
PDIEchoMonitorNode::send_echo_and_measure_delay(int64_t timeout) const
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
    throw EchoFlagTimeout(ERS_HERE, timeout);
  }

  auto time_rx_l = getNode("csr.rx_l").read();
  auto time_rx_h = getNode("csr.rx_h").read();

  auto time_tx_l = getNode("csr.tx_l").read();
  auto time_tx_h = getNode("csr.tx_h").read();

  getClient().dispatch();

  uint64_t time_rx = ((uint64_t)time_rx_h.value() << 32) + time_rx_l.value(); // NOLINT(build/unsigned)
  uint64_t time_tx = ((uint64_t)time_tx_h.value() << 32) + time_tx_l.value(); // NOLINT(build/unsigned)

  TLOG_DEBUG(0) << "tx ts: " << format_reg_value(time_tx);
  TLOG_DEBUG(0) << "rx ts: " << format_reg_value(time_rx);

  return time_rx - time_tx;
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq