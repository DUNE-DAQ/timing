/**
 * @file MasterNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/MasterNode.hpp"

#include <string>

namespace dunedaq {
namespace timing {

//-----------------------------------------------------------------------------
MasterNode::MasterNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
MasterNode::~MasterNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint64_t // NOLINT(build/unsigned)
MasterNode::read_timestamp() const
{
  return getNode<TimestampGeneratorNode>("tstamp").read_timestamp();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MasterNode::set_timestamp(uint64_t timestamp) const // NOLINT(build/unsigned)
{
  getNode<TimestampGeneratorNode>("tstamp").set_timestamp(timestamp);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
const PartitionNode&
MasterNode::get_partition_node(uint32_t partition_id) const // NOLINT(build/unsigned)
{
  const std::string nodeName = "partition" + std::to_string(partition_id);
  return getNode<PartitionNode>(nodeName);
}

//-----------------------------------------------------------------------------
void
MasterNode::apply_endpoint_delay(const ActiveEndpointConfig& ept_config, bool measure_rtt) const
{
  std::string lEptIdD = ept_config.id;
  uint32_t lEptAdr = ept_config.adr;    // NOLINT(build/unsigned)
  uint32_t lCDelay = ept_config.cdelay; // NOLINT(build/unsigned)
  uint32_t lFDelay = ept_config.fdelay; // NOLINT(build/unsigned)
  uint32_t lPDelay = ept_config.pdelay; // NOLINT(build/unsigned)
  apply_endpoint_delay(lEptAdr, lCDelay, lFDelay, lPDelay, measure_rtt);
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq