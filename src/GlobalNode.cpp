#include "pdt/GlobalNode.hpp"

namespace dunedaq {
namespace pdt {

UHAL_REGISTER_DERIVED_NODE(GlobalNode)

//-----------------------------------------------------------------------------
GlobalNode::GlobalNode(const uhal::Node& aNode) : TimingNode(aNode) {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
GlobalNode::~GlobalNode() {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool GlobalNode::in_spill() const {
    return false;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool GlobalNode::tx_error() const {
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
std::string GlobalNode::get_status(bool aPrint) const {
	std::stringstream lStatus;
	auto subnodes = read_sub_nodes(getNode("csr.stat"));
    lStatus << formatRegTable(subnodes, "Global state");
    if (aPrint) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
GlobalNode::enableUpstreamEndpoint(uint32_t aTimeout) {
	getNode("csr.ctrl.ep_en").write(0x0);
	getClient().dispatch();
	getNode("csr.ctrl.ep_en").write(0x1);
	getClient().dispatch();

	ERS_INFO("Upstream endpoint reset, waiting for lock");

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

        if (lEptRdy.value()) {
        	ERS_INFO("Endpoint locked: state= " << formatRegValue(lEptStat));
            return;
        }
	}
	
	if (!lEptRdy.value()) {
		throw UpstreamEndpointFailedToLock(ERS_HERE, getId(), formatRegValue(lEptStat));
    } else {
        ERS_INFO("Endpoint locked: state= " << formatRegValue(lEptStat));
    }
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq