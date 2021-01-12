#include "pdt/SIMIONode.hpp"

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
SIMIONode::getStatus(bool aPrint) const {
	std::stringstream lStatus;

	auto subnodes = readSubNodes(getNode("csr.stat"));
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
	} catch(...) {
		std::ostringstream lMsg;
        lMsg << "Board type not in board type map: " << lBoardType;
        throw MissingBoardTypeMapEntry(lMsg.str());
	}

	try {
		lHardwareInfo.push_back(std::make_pair("Board revision", kBoardRevisionMap.at(lBoardRevision)));
	} catch(...) {
		std::ostringstream lMsg;
        lMsg << "Board revision not in board revision map: " << lBoardRevision;
        throw MissingBoardRevisionMapEntry(lMsg.str());
	}

	try {
		lHardwareInfo.push_back(std::make_pair("Carrier type", kCarrierTypeMap.at(lCarrierType)));
	} catch(...) {
		std::ostringstream lMsg;
        lMsg << "Carrier type not in carrier type map: " << lCarrierType;
        throw MissingCarrierTypeMapEntry(lMsg.str());
	}

	try {
		lHardwareInfo.push_back(std::make_pair("Design type", kDesignTypeMap.at(lDesignType)));
	} catch(...) {
		std::ostringstream lMsg;
        lMsg << "Design type not in design type map: " << lDesignType;
        throw MissingDesignTypeMapEntry(lMsg.str());
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
	PDT_LOG(kInfo) << "Reset done";
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
SIMIONode::configurePLL(const std::string& /*aClockConfigFile*/) const {
	PDT_LOG(kWarning) << "Simulation does not support PLL config";
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<double>
SIMIONode::readClockFrequencies() const {
	PDT_LOG(kWarning) << "Simulation does not support reading of freq";
	return {};
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
SIMIONode::getClockFrequenciesTable(bool /*aPrint*/) const {
	PDT_LOG(kWarning) << "Simulation does not support freq table";
	return "";
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
SIMIONode::getPLLStatus(bool /*aPrint*/) const {
	PDT_LOG(kWarning) << "Simulation does not support PLL status";
	return "";
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
SIMIONode::getSFPStatus(uint32_t /*aSFPId*/, bool /*aPrint*/) const {
	PDT_LOG(kWarning) << "Simulation does not support SFP I2C";
	return "";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SIMIONode::switchSFPSoftTxControlBit(uint32_t /*aSFPId*/, bool /*aOn*/) const {
	PDT_LOG(kWarning) << "Simulation does not support SFP I2C";
}
//-----------------------------------------------------------------------------
} // namespace pdt