#include "pdt/EchoMonitorNode.hpp"

namespace dunedaq {
namespace pdt {

UHAL_REGISTER_DERIVED_NODE(EchoMonitorNode)

//-----------------------------------------------------------------------------
EchoMonitorNode::EchoMonitorNode(const uhal::Node& node) : TimingNode(node) {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
EchoMonitorNode::~EchoMonitorNode() {

}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
EchoMonitorNode::get_status(bool print_out) const {
	std::stringstream lStatus;
	auto subnodes = read_sub_nodes(getNode("csr.stat"));
    lStatus << format_reg_table(subnodes, "Echo mon state");
    if (print_out) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint64_t
EchoMonitorNode::send_echo_and_measure_delay(int64_t timeout) const {
	
	getNode("csr.ctrl.go").write(0x1);
    getClient().dispatch();

	auto start = std::chrono::high_resolution_clock::now();

	std::chrono::milliseconds msSinceStart(0);

	uhal::ValWord<uint32_t> lDone;

	while (msSinceStart.count() < timeout) {

		auto now = std::chrono::high_resolution_clock::now();
		msSinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

		millisleep(100);
		
		lDone = getNode("csr.stat.rx_done").read();
        getClient().dispatch();
		
		TLOG_DEBUG(0) << "rx done: " << std::hex << lDone.value();

		if (lDone.value()) break; 
	}

	if (!lDone.value()) {
        throw EchoTimeout(ERS_HERE, getId(), timeout);
	}

	auto lTimeRxL = getNode("csr.rx_l").read();
	auto lTimeRxH = getNode("csr.rx_h").read();
	auto lTimeTxL = getNode("csr.tx_l").read();
	auto lTimeTxH = getNode("csr.tx_h").read();
    
    getClient().dispatch();

    uint64_t lTimeRx = ((uint64_t)lTimeRxH.value() << 32) + lTimeRxL.value();
    uint64_t lTimeTx = ((uint64_t)lTimeTxH.value() << 32) + lTimeTxL.value();

    TLOG_DEBUG(0) << "tx ts: " << format_reg_value(lTimeTx);
    TLOG_DEBUG(0) << "rx ts: " << format_reg_value(lTimeRx);

    return lTimeRx - lTimeTx;
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq