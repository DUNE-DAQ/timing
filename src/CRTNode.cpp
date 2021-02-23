#include "pdt/CRTNode.hpp"

namespace dunedaq {
namespace pdt {

UHAL_REGISTER_DERIVED_NODE(CRTNode)

//-----------------------------------------------------------------------------
CRTNode::CRTNode(const uhal::Node& node) : TimingNode(node) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
CRTNode::~CRTNode() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
CRTNode::enable(uint32_t partition, uint32_t command) const {

	getNode("csr.ctrl.tgrp").write(partition);
    getNode("pulse.ctrl.cmd").write(command);
    getNode("pulse.ctrl.en").write(0x1);
	getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
CRTNode::disable() const {
	getNode("pulse.ctrl.en").write(0x0);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
CRTNode::get_status(bool print_out) const {
	std::stringstream lStatus;
	auto lCRTRegs = read_sub_nodes(getNode(""));
    lStatus << format_reg_table(lCRTRegs, "CRT state", {"", ""}) << std::endl;

    const uint64_t lLastPulseTimestamp = ((uint64_t)lCRTRegs.at("pulse.ts_h").value() << 32) + lCRTRegs.at("pulse.ts_l").value();
    lStatus << "Last Pulse Timestamp: 0x" << std::hex << lLastPulseTimestamp << std::endl;

    if (print_out) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint64_t
CRTNode::read_last_pulse_timestamp() const {

    auto lTimestampRegLow = getNode("pulse.ts_l").read();
    auto lTimestampRegHigh = getNode("pulse.ts_h").read();
    getClient().dispatch();

    return ((uint64_t)lTimestampRegHigh.value() << 32) + lTimestampRegLow.value();	
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq