/**
 * @file IRIGTimestampNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/IRIGTimestampNode.hpp"

#include "timing/toolbox.hpp"
#include "logging/Logging.hpp"

#include <string>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(IRIGTimestampNode)

//-----------------------------------------------------------------------------
IRIGTimestampNode::IRIGTimestampNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
IRIGTimestampNode::~IRIGTimestampNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
IRIGTimestampNode::get_status(bool print_out) const
{
  std::stringstream status;
  status << "Timestamp: 0x" << std::hex << read_timestamp() << std::endl;

  auto ctrl_subnodes = read_sub_nodes(getNode("csr.ctrl"));
  status << format_reg_table(ctrl_subnodes, "IRIG ts ctrl");

  auto stat_subnodes = read_sub_nodes(getNode("csr.stat"));
  status << format_reg_table(stat_subnodes, "IRIG ts state");

  auto date_subnodes = read_sub_nodes(getNode("csr.irig_date"));
  status << format_reg_table(date_subnodes, "IRIG date");

  auto time_subnodes = read_sub_nodes(getNode("csr.irig_time"));
  status << format_reg_table(time_subnodes, "IRIG time");

  auto sbs_subnodes = read_sub_nodes(getNode("csr.irig_sbs"));
  status << format_reg_table(sbs_subnodes, "IRIG SBS");

  status << "PPS counter: 0x" << std::hex << read_pps_counter() << std::endl;

  status << "Seconds since epoch: 0x" << std::hex << read_seconds_since_epoch() << std::endl;

  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uhal::ValVector<uint32_t> // NOLINT(build/unsigned)
IRIGTimestampNode::read_raw_timestamp(bool dispatch) const
{
  auto timestamp = getNode("tstamp").readBlock(2);
  if (dispatch)
    getClient().dispatch();
  return timestamp;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint64_t // NOLINT(build/unsigned)
IRIGTimestampNode::read_timestamp() const
{
  return tstamp2int(read_raw_timestamp());
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
IRIGTimestampNode::set_ts_timebase(TimestampTimebase timebase) const // NOLINT(build/unsigned)
{
  getNode("csr.ctrl.ts_timebase").write(timebase);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
IRIGTimestampNode::set_ts_epoch(TimestampEpoch epoch) const // NOLINT(build/unsigned)
{
  getNode("csr.ctrl.ts_epoch").write(epoch);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
IRIGTimestampNode::set_ts_epoch_value(uint64_t epoch_to_2000_seconds_tai, uint8_t epoch_to_2000_leap_seconds) const // NOLINT(build/unsigned)
{
  uint32_t epoch_l = epoch_to_2000_seconds_tai;
  uint32_t epoch_h = epoch_to_2000_seconds_tai >> 32;
  getNode("csr.seconds_from_sw_epoch_l").write(epoch_l);
  getNode("csr.seconds_from_sw_epoch_h").write(epoch_h);
  getNode("csr.offsets.leap_seconds_from_sw_epoch").write(epoch_to_2000_leap_seconds);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
IRIGTimestampNode::read_pps_counter() const
{
  auto counter = getNode("pps_ctr").read();
  getClient().dispatch();
  return counter.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint64_t // NOLINT(build/unsigned)
IRIGTimestampNode::read_seconds_since_epoch() const
{
  auto seconds_since_epoch = getNode("seconds_since_epoch").readBlock(2);
  getClient().dispatch();
  return tstamp2int(seconds_since_epoch);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
IRIGTimestampNode::set_ts_seconds_offset(int8_t seconds_offset) const // NOLINT(build/signed)
{
  // cast to uint8_t to avoid erroneous auto conversion
  getNode("csr.offsets.seconds_offset").write((uint8_t)seconds_offset);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
IRIGTimestampNode::set_ts_ticks_offset(int16_t ticks_offset) const // NOLINT(build/signed)
{
  // cast to uint16_t to avoid erroneous auto conversion
  getNode("csr.offsets.ticks_offset").write((uint16_t)ticks_offset); // NOLINT(build/unsigned)
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq