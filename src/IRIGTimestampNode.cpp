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
IRIGTimestampNode::set_irig_epoch(IRIGEpoch irig_epoch) const // NOLINT(build/unsigned)
{
  getNode("csr.ctrl.rst").write(0x1);
  getNode("csr.ctrl.irig_epoch").write(irig_epoch);
  getNode("csr.ctrl.rst").write(0x0);
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
} // namespace timing
} // namespace dunedaq