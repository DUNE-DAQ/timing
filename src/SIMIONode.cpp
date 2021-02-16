#include "pdt/SIMIONode.hpp"

namespace dunedaq {
namespace pdt {

UHAL_REGISTER_DERIVED_NODE(SIMIONode)

//-----------------------------------------------------------------------------
SIMIONode::SIMIONode(const uhal::Node& aNode) : 
IONode(aNode, "", "", "", "", {}, {}) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SIMIONode::~SIMIONode() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string 
SIMIONode::get_status(bool aPrint) const {
	std::stringstream lStatus;

	auto subnodes = read_sub_nodes(getNode("csr.stat"));
	lStatus << format_reg_table(subnodes, "SIM IO state");

	if (aPrint) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint64_t
SIMIONode::read_board_uid() const {
	return 0;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
BoardRevision
SIMIONode::get_board_revision() const {
	return kSIMRev1;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
SIMIONode::get_hardware_info(bool aPrint) const {
	std::stringstream lInfo;
	const BoardType lBoardType = convert_value_to_board_type(read_board_type());
	const BoardRevision lBoardRevision = get_board_revision();
	const CarrierType lCarrierType = convert_value_to_carrier_type(read_carrier_type());
	const DesignType lDesignType = convert_value_to_design_type(read_design_type());

	std::vector<std::pair<std::string, std::string>> lHardwareInfo;

	// TODO check map at exception
	try {
		lHardwareInfo.push_back(std::make_pair("Board type", kBoardTypeMap.at(lBoardType)));
	} catch(const std::out_of_range& e) {
        throw MissingBoardTypeMapEntry(ERS_HERE, getId(), format_reg_value(lBoardType), e);
	}

	try {
		lHardwareInfo.push_back(std::make_pair("Board revision", kBoardRevisionMap.at(lBoardRevision)));
	} catch(const std::out_of_range& e) {
        throw MissingBoardRevisionMapEntry(ERS_HERE, getId(), format_reg_value(lBoardRevision), e);
	}

	try {
		lHardwareInfo.push_back(std::make_pair("Carrier type", kCarrierTypeMap.at(lCarrierType)));
	} catch(const std::out_of_range& e) {
        throw MissingCarrierTypeMapEntry(ERS_HERE, getId(), format_reg_value(lCarrierType), e);
	}

	try {
		lHardwareInfo.push_back(std::make_pair("Design type", kDesignTypeMap.at(lDesignType)));
	} catch(const std::out_of_range& e) {
        throw MissingDesignTypeMapEntry(ERS_HERE, getId(), format_reg_value(lDesignType), e);
	}
	lInfo << format_reg_table(lHardwareInfo, "Hardware info", {"", ""});

	if (aPrint) std::cout << lInfo.str();
	return lInfo.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SIMIONode::reset(const std::string& /*aClockConfigFile*/) const {

	writeSoftResetRegister();	
	ERS_INFO("Reset done");
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
SIMIONode::configure_pll(const std::string& /*aClockConfigFile*/) const {
	ERS_INFO("Simulation does not support PLL config");
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<double>
SIMIONode::read_clock_frequencies() const {
	ERS_INFO("Simulation does not support reading of freq");
	return {};
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
SIMIONode::get_clock_frequencies_table(bool /*aPrint*/) const {
	ERS_INFO("Simulation does not support freq table");
	return "Simulation does not support freq table";
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
SIMIONode::get_pll_status(bool /*aPrint*/) const {
	ERS_INFO("Simulation does not support PLL status");
	return "Simulation does not support PLL status";
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
SIMIONode::get_sfp_status(uint32_t /*aSFPId*/, bool /*aPrint*/) const {
	ERS_INFO("Simulation does not support SFP I2C");
	return "Simulation does not support SFP I2C";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SIMIONode::switch_sfp_soft_tx_control_bit(uint32_t /*aSFPId*/, bool /*aOn*/) const {
	ERS_INFO("Simulation does not support SFP I2C");
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq