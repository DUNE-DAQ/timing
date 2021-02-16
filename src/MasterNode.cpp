#include "pdt/MasterNode.hpp"

namespace dunedaq {
namespace pdt {

//-----------------------------------------------------------------------------
MasterNode::MasterNode(const uhal::Node& aNode) : TimingNode(aNode) {
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
MasterNode::set_timestamp(uint64_t aTimestamp) const {
	getNode<TimestampGeneratorNode>("master.tstamp").set_timestamp(aTimestamp);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
const PartitionNode&
MasterNode::get_partition_node(uint32_t aPartID) const {
	const std::string nodeName = "partition" + std::to_string(aPartID);
	return getNode<PartitionNode>(nodeName);
}

//-----------------------------------------------------------------------------
void
MasterNode::apply_endpoint_delay(const ActiveEndpointConfig& aEptConfig, bool aMeasureRTT) const {
	std::string lEptIdD = aEptConfig.id;
	uint32_t lEptAdr = aEptConfig.adr;
	uint32_t lCDelay = aEptConfig.cdelay;
	uint32_t lFDelay = aEptConfig.fdelay;
	uint32_t lPDelay = aEptConfig.pdelay;
	apply_endpoint_delay(lEptAdr, lCDelay, lFDelay, lPDelay, aMeasureRTT);
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq