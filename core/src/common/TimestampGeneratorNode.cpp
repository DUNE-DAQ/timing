#include "pdt/TimestampGeneratorNode.hpp"

namespace pdt {

UHAL_REGISTER_DERIVED_NODE(TimestampGeneratorNode);

//-----------------------------------------------------------------------------
TimestampGeneratorNode::TimestampGeneratorNode(const uhal::Node& aNode) : TimingNode(aNode) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
TimestampGeneratorNode::~TimestampGeneratorNode() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string 
TimestampGeneratorNode::getStatus(bool aPrint) const {
	std::stringstream lStatus;
	lStatus << "Timestamp: 0x" << std::hex << readTimestamp() << std::endl;
	if (aPrint) std::cout << lStatus.str();
	return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uhal::ValVector<uint32_t>
TimestampGeneratorNode::readRawTimestamp(bool aDispatch) const {
	auto lTimestamp = getNode("ctr.val").readBlock(2);
	if (aDispatch) getClient().dispatch();
    return lTimestamp;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint64_t
TimestampGeneratorNode::readTimestamp() const {
    return tstamp2int(readRawTimestamp());
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
TimestampGeneratorNode::setTimestamp(uint64_t aTimestamp) const {
	// Take the timestamp and split it up
	uint32_t lNowH = (aTimestamp >> 32) & ((1UL<<32)-1);
    uint32_t lNowL = (aTimestamp >> 0) & ((1UL<<32)-1);
    getNode("ctr.set").writeBlock({lNowL, lNowH});
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

} // namespace pdt