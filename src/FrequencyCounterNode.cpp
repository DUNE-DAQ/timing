#include "timing/FrequencyCounterNode.hpp"

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(FrequencyCounterNode)

//-----------------------------------------------------------------------------
FrequencyCounterNode::FrequencyCounterNode(const uhal::Node& node) : TimingNode(node) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
FrequencyCounterNode::~FrequencyCounterNode() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
FrequencyCounterNode::get_status(bool print_out) const {
	std::stringstream lStatus;
	auto subnodes = read_sub_nodes(getNode("csr.ctrl"));
    lStatus << format_reg_table(subnodes, "Freq counter state");
    if (print_out) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<double>
FrequencyCounterNode::measure_frequencies(uint8_t number_of_clocks) const {
	std::vector<double> lFrequencies;

	for (uint8_t i=0; i < number_of_clocks; ++i) {
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

} // namespace timing
} // namespace dunedaq