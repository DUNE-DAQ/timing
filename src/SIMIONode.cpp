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
	lStatus << formatRegTable(subnodes, "SIM IO state");

	if (aPrint) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint64_t
SIMIONode::readBoardUID() const {
	return 0;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
BoardRevision
SIMIONode::getBoardRevision() const {
	return kSIMRev1;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
SIMIONode::getHardwareInfo(bool aPrint) const {
	std::stringstream lInfo;
	const BoardType lBoardType = convertValueToBoardType(readBoardType());
	const BoardRevision lBoardRevision = getBoardRevision();
	const CarrierType lCarrierType = convertValueToCarrierType(readCarrierType());
	const DesignType lDesignType = convertValueToDesignType(readDesignType());

	std::vector<std::pair<std::string, std::string>> lHardwareInfo;

	// TODO check map at exception
	try {
		lHardwareInfo.push_back(std::make_pair("Board type", kBoardTypeMap.at(lBoardType)));
	} catch(const std::out_of_range& e) {
        throw MissingBoardTypeMapEntry(ERS_HERE, getId(), formatRegValue(lBoardType), e);
	}

	try {
		lHardwareInfo.push_back(std::make_pair("Board revision", kBoardRevisionMap.at(lBoardRevision)));
	} catch(const std::out_of_range& e) {
        throw MissingBoardRevisionMapEntry(ERS_HERE, getId(), formatRegValue(lBoardRevision), e);
	}

	try {
		lHardwareInfo.push_back(std::make_pair("Carrier type", kCarrierTypeMap.at(lCarrierType)));
	} catch(const std::out_of_range& e) {
        throw MissingCarrierTypeMapEntry(ERS_HERE, getId(), formatRegValue(lCarrierType), e);
	}

	try {
		lHardwareInfo.push_back(std::make_pair("Design type", kDesignTypeMap.at(lDesignType)));
	} catch(const std::out_of_range& e) {
        throw MissingDesignTypeMapEntry(ERS_HERE, getId(), formatRegValue(lDesignType), e);
	}
	lInfo << formatRegTable(lHardwareInfo, "Hardware info", {"", ""});

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
SIMIONode::configurePLL(const std::string& /*aClockConfigFile*/) const {
	ERS_INFO("Simulation does not support PLL config");
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<double>
SIMIONode::readClockFrequencies() const {
	ERS_INFO("Simulation does not support reading of freq");
	return {};
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
SIMIONode::getClockFrequenciesTable(bool /*aPrint*/) const {
	ERS_INFO("Simulation does not support freq table");
	return "Simulation does not support freq table";
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
SIMIONode::getPLLStatus(bool /*aPrint*/) const {
	ERS_INFO("Simulation does not support PLL status");
	return "Simulation does not support PLL status";
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
SIMIONode::getSFPStatus(uint32_t /*aSFPId*/, bool /*aPrint*/) const {
	ERS_INFO("Simulation does not support SFP I2C");
	return "Simulation does not support SFP I2C";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SIMIONode::switchSFPSoftTxControlBit(uint32_t /*aSFPId*/, bool /*aOn*/) const {
	ERS_INFO("Simulation does not support SFP I2C");
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq