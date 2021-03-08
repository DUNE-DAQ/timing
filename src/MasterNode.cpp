#include "timing/MasterNode.hpp"

namespace dunedaq {
namespace timing {

//-----------------------------------------------------------------------------
MasterNode::MasterNode(const uhal::Node& node) : TimingNode(node) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
MasterNode::~MasterNode() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint64_t
MasterNode::read_timestamp() const {
	return getNode<TimestampGeneratorNode>("master.tstamp").read_timestamp();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
MasterNode::set_timestamp(uint64_t timestamp) const {
	getNode<TimestampGeneratorNode>("master.tstamp").set_timestamp(timestamp);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
const PartitionNode&
MasterNode::get_partition_node(uint32_t partition_id) const {
	const std::string nodeName = "partition" + std::to_string(partition_id);
	return getNode<PartitionNode>(nodeName);
}

//-----------------------------------------------------------------------------
void
MasterNode::apply_endpoint_delay(const ActiveEndpointConfig& ept_config, bool measure_rtt) const {
	std::string lEptIdD = ept_config.id;
	uint32_t lEptAdr = ept_config.adr;
	uint32_t lCDelay = ept_config.cdelay;
	uint32_t lFDelay = ept_config.fdelay;
	uint32_t lPDelay = ept_config.pdelay;
	apply_endpoint_delay(lEptAdr, lCDelay, lFDelay, lPDelay, measure_rtt);
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq