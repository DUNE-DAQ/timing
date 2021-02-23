#include "pdt/TimestampGeneratorNode.hpp"

namespace dunedaq {
namespace pdt {

UHAL_REGISTER_DERIVED_NODE(TimestampGeneratorNode)

//-----------------------------------------------------------------------------
TimestampGeneratorNode::TimestampGeneratorNode(const uhal::Node& node) : TimingNode(node) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
TimestampGeneratorNode::~TimestampGeneratorNode() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string 
TimestampGeneratorNode::get_status(bool print_out) const {
	std::stringstream lStatus;
	lStatus << "Timestamp: 0x" << std::hex << read_timestamp() << std::endl;
	if (print_out) std::cout << lStatus.str();
	return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uhal::ValVector<uint32_t>
TimestampGeneratorNode::read_raw_timestamp(bool dispatch) const {
	auto lTimestamp = getNode("ctr.val").readBlock(2);
	if (dispatch) getClient().dispatch();
    return lTimestamp;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint64_t
TimestampGeneratorNode::read_timestamp() const {
    return tstamp2int(read_raw_timestamp());
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
TimestampGeneratorNode::set_timestamp(uint64_t timestamp) const {
	// Take the timestamp and split it up
	uint32_t lNowH = (timestamp >> 32) & ((1UL<<32)-1);
    uint32_t lNowL = (timestamp >> 0) & ((1UL<<32)-1);
    getNode("ctr.set").writeBlock({lNowL, lNowH});
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq