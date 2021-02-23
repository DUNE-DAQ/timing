#include "pdt/GlobalNode.hpp"

namespace dunedaq {
namespace pdt {

UHAL_REGISTER_DERIVED_NODE(GlobalNode)

//-----------------------------------------------------------------------------
GlobalNode::GlobalNode(const uhal::Node& node) : TimingNode(node) {

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
uint32_t GlobalNode::read_spill_counter() const {
    return 0;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void GlobalNode::select_partition() const {

}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void GlobalNode::lock_partition() const {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string GlobalNode::get_status(bool print_out) const {
	std::stringstream lStatus;
	auto subnodes = read_sub_nodes(getNode("csr.stat"));
    lStatus << format_reg_table(subnodes, "Global state");
    if (print_out) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
GlobalNode::enable_upstream_endpoint(uint32_t timeout) {
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
	while (msSinceStart.count() < timeout) {
		auto now = std::chrono::high_resolution_clock::now();
		msSinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

		millisleep(100);
		
		lEptStat = getNode("csr.stat.ep_stat").read();
		lEptRdy = getNode("csr.stat.ep_rdy").read();
        
        getClient().dispatch();

        if (lEptRdy.value()) {
        	ERS_INFO("Endpoint locked: state= " << format_reg_value(lEptStat));
            return;
        }
	}
	
	if (!lEptRdy.value()) {
		throw UpstreamEndpointFailedToLock(ERS_HERE, getId(), format_reg_value(lEptStat));
    } else {
        ERS_INFO("Endpoint locked: state= " << format_reg_value(lEptStat));
    }
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq