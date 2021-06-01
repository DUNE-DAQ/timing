#include "pdt/GlobalNode.hpp"

namespace pdt {

UHAL_REGISTER_DERIVED_NODE(GlobalNode);

//-----------------------------------------------------------------------------
GlobalNode::GlobalNode(const uhal::Node& aNode) : TimingNode(aNode) {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
GlobalNode::~GlobalNode() {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool GlobalNode::inSpill() const {
    return false;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool GlobalNode::txError() const {
    return false;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t GlobalNode::readTimeStamp() const {
    return 0;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t GlobalNode::readSpillCounter() const {
    return 0;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void GlobalNode::selectPartition() const {

}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void GlobalNode::lockPartition() const {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string GlobalNode::getStatus(bool aPrint) const {
	std::stringstream lStatus;
	auto subnodes = readSubNodes(getNode("csr.stat"));
    lStatus << formatRegTable(subnodes, "Global state");
    if (aPrint) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool
GlobalNode::enableUpstreamEndpoint(uint32_t aTimeout) {
	getNode("csr.ctrl.ep_en").write(0x0);
	getClient().dispatch();
	getNode("csr.ctrl.ep_en").write(0x1);
	getClient().dispatch();

	PDT_LOG(kNotice) << "Upstream endpoint reset, waiting for lock";

	auto start = std::chrono::high_resolution_clock::now();

	std::chrono::milliseconds msSinceStart(0);

	uhal::ValWord<uint32_t> lEptStat;
	uhal::ValWord<uint32_t> lEptRdy;

	// Wait for the endpoint to be happy
	while (msSinceStart.count() < aTimeout) {
		auto now = std::chrono::high_resolution_clock::now();
		msSinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

		millisleep(100);
		
		lEptStat = getNode("csr.stat.ep_stat").read();
		lEptRdy = getNode("csr.stat.ep_rdy").read();
        
        getClient().dispatch();
		
		PDT_LOG(kNotice) << "ept state: 0x" << std::hex << lEptStat.value() << " ept ready: 0x" << lEptRdy.value();

        if (lEptRdy.value() && lEptStat.value() == 0x8) {
        	PDT_LOG(kNotice) << "Endpoint locked: state= " << formatRegValue(lEptStat.value());
            return true;
        }
	}
	
	if (!lEptRdy.value() || lEptStat.value() != 0x8) {
		PDT_LOG(kError) << "Failed to bring up the RTT endpoint. Current state: 0x" << std::hex << lEptStat.value();
		return false;
    } else {
        PDT_LOG(kNotice) << "Endpoint locked";
        return true;
    }
}
//-----------------------------------------------------------------------------
} // namespace pdt