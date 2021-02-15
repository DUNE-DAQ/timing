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
MasterNode::readTimestamp() const {
	return getNode<TimestampGeneratorNode>("master.tstamp").readTimestamp();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
MasterNode::setTimestamp(uint64_t aTimestamp) const {
	getNode<TimestampGeneratorNode>("master.tstamp").setTimestamp(aTimestamp);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
const PartitionNode&
MasterNode::getPartitionNode(uint32_t aPartID) const {
	const std::string nodeName = "partition" + std::to_string(aPartID);
	return getNode<PartitionNode>(nodeName);
}

//-----------------------------------------------------------------------------
void
MasterNode::applyEndpointDelay(const ActiveEndpointConfig& aEptConfig, bool aMeasureRTT) const {
	std::string lEptIdD = aEptConfig.id;
	uint32_t lEptAdr = aEptConfig.adr;
	uint32_t lCDelay = aEptConfig.cdelay;
	uint32_t lFDelay = aEptConfig.fdelay;
	uint32_t lPDelay = aEptConfig.pdelay;
	applyEndpointDelay(lEptAdr, lCDelay, lFDelay, lPDelay, aMeasureRTT);
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq