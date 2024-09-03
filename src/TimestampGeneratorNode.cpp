/**
 * @file TimestampGeneratorNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/TimestampGeneratorNode.hpp"

#include "timing/toolbox.hpp"
#include "logging/Logging.hpp"

#include <string>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(TimestampGeneratorNode)

//-----------------------------------------------------------------------------
TimestampGeneratorNode::TimestampGeneratorNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
TimestampGeneratorNode::~TimestampGeneratorNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
TimestampGeneratorNode::get_status(bool print_out) const
{
  std::stringstream status;
  status << "Current timestamp: 0x" << std::hex << read_timestamp() << std::endl;
  status << "Start timestamp:   0x" << std::hex << read_start_timestamp() << std::endl;
  status << "SW init timestamp: 0x" << std::hex << read_sw_init_timestamp() << std::endl;

  auto ctrl_subnodes = read_sub_nodes(getNode("csr.ctrl"));
  status << format_reg_table(ctrl_subnodes, "TS gen ctrl");

  auto stat_subnodes = read_sub_nodes(getNode("csr.stat"));
  status << format_reg_table(stat_subnodes, "TS gen state");


  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uhal::ValVector<uint32_t> // NOLINT(build/unsigned)
TimestampGeneratorNode::read_raw_timestamp(bool dispatch) const
{
  auto timestamp = getNode("ctr").readBlock(2);
  if (dispatch)
    getClient().dispatch();
  return timestamp;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint64_t // NOLINT(build/unsigned)
TimestampGeneratorNode::read_timestamp() const
{
  return tstamp2int(read_raw_timestamp());
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint64_t // NOLINT(build/unsigned)
TimestampGeneratorNode::read_start_timestamp() const
{
  auto start_ts_l = getNode("csr.tstamp_start_l").read();
  auto start_ts_h = getNode("csr.tstamp_start_h").read();
  getClient().dispatch();
  return (uint64_t)start_ts_l.value() + ((uint64_t)start_ts_h.value() << 32);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint64_t // NOLINT(build/unsigned)
TimestampGeneratorNode::read_sw_init_timestamp() const
{
  auto sw_init_ts_l = getNode("csr.tstamp_sw_init_l").read();
  auto sw_init_ts_h = getNode("csr.tstamp_sw_init_h").read();
  getClient().dispatch();
  return (uint64_t)sw_init_ts_l.value() + ((uint64_t)sw_init_ts_h.value() << 32);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
TimestampGeneratorNode::set_timestamp(TimestampSource source) const // NOLINT(build/unsigned)
{
  // TODO put somewhere more accessible
  const uint clock_frequency_hz = 62500000;

  const uint64_t old_timestamp = read_timestamp(); // NOLINT(build/unsigned)
  TLOG() << "Reading old timestamp: " << format_reg_value(old_timestamp) << ", " << format_timestamp(old_timestamp, clock_frequency_hz);

  getNode("csr.ctrl.tstamp_source_sel").write(source);

  uint64_t now_timestamp;
  if (source == kSoftware)
  {
    now_timestamp = get_milliseconds_since_epoch() * (clock_frequency_hz / 1000); // NOLINT(build/unsigned)
  }
  else if (source == kMixed)
  {
    now_timestamp = get_seconds_since_epoch() * clock_frequency_hz ; // NOLINT(build/unsigned)
  }

  if (source != kUpstream)
  {
    TLOG() << "New software timestamp: " << format_reg_value(now_timestamp) << ", " << format_timestamp(now_timestamp, clock_frequency_hz);
    // Take the timestamp and split it up
    uint32_t now_ts_low = (now_timestamp >> 0) & ((1UL << 32) - 1);  // NOLINT(build/unsigned)
    uint32_t now_ts_high = (now_timestamp >> 32) & ((1UL << 32) - 1); // NOLINT(build/unsigned)

    getNode("csr.tstamp_sw_init_l").write(now_ts_low);
    getNode("csr.tstamp_sw_init_h").write(now_ts_high);
  }

  getNode("csr.ctrl.tstamp_load").write(0x1);
  getNode("csr.ctrl.tstamp_load").write(0x0);
  getClient().dispatch();

  auto start = std::chrono::high_resolution_clock::now();
  while (true) {
    auto ts_loaded = getNode("csr.stat.tstamp_loaded").read();
    auto ts_error = getNode("csr.stat.tstamp_error").read();
    getClient().dispatch();

    TLOG_DEBUG(6) << std::hex << "ts loaded: 0x" << ts_loaded.value() << ", ts error: " << ts_error.value();

    if (ts_loaded.value() && !ts_error.value())
    {
      const uint64_t start_ts = read_start_timestamp();
      TLOG() << "Timestamp initialised with: " << format_reg_value(start_ts) << ", " << format_timestamp(start_ts, clock_frequency_hz);
      break;
    }

    auto now = std::chrono::high_resolution_clock::now();
    auto ms_since_start = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

    if (ms_since_start.count() > 1000)
      throw ReceiverNotReady(ERS_HERE, ts_loaded.value(), ts_error.value());

    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }

  const uint64_t new_timestamp = read_timestamp(); // NOLINT(build/unsigned)
  TLOG() << "Reading new timestamp: " << format_reg_value(new_timestamp) << ", " << format_timestamp(new_timestamp, clock_frequency_hz);

  getClient().dispatch();
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq