#include "pdt/FrequencyCounterNode.hpp"

namespace pdt {

UHAL_REGISTER_DERIVED_NODE(FrequencyCounterNode)

//-----------------------------------------------------------------------------
FrequencyCounterNode::FrequencyCounterNode(const uhal::Node& aNode) : TimingNode(aNode) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
FrequencyCounterNode::~FrequencyCounterNode() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
FrequencyCounterNode::getStatus(bool aPrint) const {
	std::stringstream lStatus;
	auto subnodes = readSubNodes(getNode("csr.ctrl"));
    lStatus << formatRegTable(subnodes, "Freq counter state");
    if (aPrint) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<double>
FrequencyCounterNode::measureFrequencies(uint8_t nClocks) const {
	std::vector<double> lFrequencies;

	for (uint8_t i=0; i < nClocks; ++i) {
		getNode("ctrl.chan_sel").write(i);
		getNode("ctrl.en_crap_mode").write(0);
		getClient().dispatch();
		
		millisleep(2000);

		uhal::ValWord<uint32_t> lFrequency = getNode("freq.count").read();
		uhal::ValWord<uint32_t> lFrequencyValid = getNode("freq.valid").read();
		getClient().dispatch();

		if (lFrequencyValid.value()) {
			double freq = lFrequency.value() * 119.20928 / 1000000;
			lFrequencies.push_back(freq);
		} else {
			lFrequencies.push_back(-1);
		}
	}
	return lFrequencies;
}
//-----------------------------------------------------------------------------

} // namespace pdt