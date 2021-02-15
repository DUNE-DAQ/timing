#include "pdt/IONode.hpp"

namespace dunedaq {
namespace pdt {

//UHAL_REGISTER_DERIVED_NODE(IONode);

//-----------------------------------------------------------------------------
IONode::IONode(const uhal::Node& aNode, std::string aUIDI2CBus, std::string aUIDI2CDevice, std::string aPLLI2CBus, std::string aPLLI2CDevice, std::vector<std::string> aClockNames, std::vector<std::string> aSFPI2CBuses) : 
	TimingNode(aNode), 
	mUIDI2CBus(aUIDI2CBus), 
	mUIDI2CDevice(aUIDI2CDevice),
	mPLLI2CBus(aPLLI2CBus),
	mPLLI2CDevice(aPLLI2CDevice),
	mClockNames(aClockNames),
	mSFPI2CBuses(aSFPI2CBuses)
	//mPLL (new SI534xSlave( getNode<I2CMasterNode>(mPLLI2CBus)& , getNode<I2CMasterNode>(mPLLI2CBus).getSlaveAddress(aPLLI2CDevice) )) 
	{
} 
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
IONode::~IONode() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
IONode::readBoardType() const {
	uhal::ValWord<uint32_t> lBoardType = getNode("config.board_type").read();
    getClient().dispatch();
    return lBoardType.value();  
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
IONode::readCarrierType() const {
	uhal::ValWord<uint32_t> lCarrierType = getNode("config.carrier_type").read();
    getClient().dispatch();
    return lCarrierType.value();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
IONode::readDesignType() const {
	uhal::ValWord<uint32_t> lDesignType = getNode("config.design_type").read();
	getClient().dispatch();
	return lDesignType.value();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint64_t
IONode::readBoardUID() const {

	uint64_t lUID = 0;
	std::vector<uint8_t> lUIDValues = getNode<I2CMasterNode>(mUIDI2CBus).getSlave(mUIDI2CDevice).readI2CArray(0xfa, 6);
	
	for (uint8_t i=0; i < lUIDValues.size(); ++i) {
		lUID = (lUID << 8) | lUIDValues.at(i);
	}
	return lUID;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
BoardRevision
IONode::getBoardRevision() const {
	
	auto lUID = readBoardUID();
	try {
		return kBoardUIDRevisionMap.at(lUID);
	} catch(const std::out_of_range& e) {
        throw UnknownBoardUID(ERS_HERE, getId(), formatRegValue(lUID), e);
	}
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
IONode::getHardwareInfo(bool aPrint) const {
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

	lHardwareInfo.push_back(std::make_pair("Board UID", formatRegValue(readBoardUID())));

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
std::string 
IONode::getFullClockConfigFilePath(const std::string& aClockConfigFile, int32_t aMode) const {

	
	if (aClockConfigFile.size()) {
		// config file provided explicitly, no need for lookup
		return aClockConfigFile;
	} else {

		std::string lConfigFile;
		std::string lClockConfigKey;

		const BoardRevision lBoardRevision = getBoardRevision();
		const CarrierType lCarrierType = convertValueToCarrierType(readCarrierType());
		const DesignType lDesignType = convertValueToDesignType(readDesignType());

		try {
			lClockConfigKey = kBoardRevisionMap.at(lBoardRevision) + "_";
		} catch(const std::out_of_range& e) {
        	throw MissingBoardRevisionMapEntry(ERS_HERE, getId(), formatRegValue(lBoardRevision), e);
		}

		try {
			lClockConfigKey = lClockConfigKey + kCarrierTypeMap.at(lCarrierType) + "_";
		} catch(const std::out_of_range& e) {
        	throw MissingCarrierTypeMapEntry(ERS_HERE, getId(), formatRegValue(lCarrierType), e);
		}

		try {
			lClockConfigKey = lClockConfigKey + kDesignTypeMap.at(lDesignType);
		} catch(const std::out_of_range& e) {
        	throw MissingDesignTypeMapEntry(ERS_HERE, getId(), formatRegValue(lDesignType), e);
		}

		// modifier in case a different clock file is needed based on firmware configuration
		if (aMode >= 0) lClockConfigKey = lClockConfigKey + "_mode" + std::to_string(aMode);

		try {
			lConfigFile = kClockConfigMap.at(lClockConfigKey);
		} catch (const std::out_of_range& e) {
        	throw ClockConfigNotFound(ERS_HERE, getId(), lClockConfigKey, e);
		}
      	
      	return std::string(std::getenv("PDT_TESTS")) + "/etc/clock/" + lConfigFile;
	}
	
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::unique_ptr<const SI534xSlave>
IONode::getPLL() const {
	return getI2CDevice<SI534xSlave>(mPLLI2CBus, mPLLI2CDevice);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void 
IONode::configurePLL(const std::string& aClockConfigFile) const {
	auto pll = getPLL();

	uint32_t lSIVersion = pll->readDeviceVersion();
	ERS_INFO("Configuring PLL        : SI" << formatRegValue(lSIVersion));

	pll->configure(aClockConfigFile);
	
	ERS_INFO("PLL configuration id   : " << pll->readConfigID());
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<double>
IONode::readClockFrequencies() const {
	return getNode<FrequencyCounterNode>("freq").measureFrequencies(mClockNames.size());
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
IONode::getClockFrequenciesTable(bool aPrint) const {
	std::stringstream lTable;
	std::vector<double> lFrequencies = readClockFrequencies();
	for (uint8_t i=0; i < lFrequencies.size(); ++i) {
		lTable << mClockNames.at(i) << " freq: " << std::setprecision (12) << lFrequencies.at(i) << std::endl;
	}
	//TODO add freq validation
	if (aPrint) std::cout << lTable.str();
	return lTable.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
IONode::getPLLStatus(bool aPrint) const {

	std::stringstream lStatus;

	auto pll = getPLL();
	lStatus << "PLL configuration id   : " << pll->readConfigID() << std::endl;

	std::map<std::string, uint32_t> lPLLVersion;
	lPLLVersion["Part number"] = pll->readDeviceVersion();
	lPLLVersion["Device grade"] = pll->readClockRegister(0x4);
	lPLLVersion["Device revision"] = pll->readClockRegister(0x5);
	
	lStatus << formatRegTable(lPLLVersion, "PLL information") << std::endl;

	std::map<std::string, uint32_t> lPLLRegisters;

	uint8_t lPLLReg_c = pll->readClockRegister(0xc);
	uint8_t lPLLReg_d = pll->readClockRegister(0xd);
	uint8_t lPLLReg_e = pll->readClockRegister(0xe);
	uint8_t lPLLReg_f = pll->readClockRegister(0xf);
	uint8_t lPLLReg_11 = pll->readClockRegister(0x11);
	uint8_t lPLLReg_12 = pll->readClockRegister(0x12);

	lPLLRegisters["CAL_PLL"] = decRng(lPLLReg_f, 5);
	lPLLRegisters["HOLD"] = decRng(lPLLReg_e, 5);
	lPLLRegisters["LOL"] = decRng(lPLLReg_e, 1);
	lPLLRegisters["LOS"] = decRng(lPLLReg_d, 0, 4);
	lPLLRegisters["LOSXAXB"] = decRng(lPLLReg_c, 1);
	lPLLRegisters["LOSXAXB_FLG"] = decRng(lPLLReg_11, 1);

	lPLLRegisters["OOF"] = decRng(lPLLReg_d, 4, 4);
    lPLLRegisters["OOF (sticky)"] = decRng(lPLLReg_12, 4, 4);

	lPLLRegisters["SMBUS_TIMEOUT"] = decRng(lPLLReg_c, 5);
	lPLLRegisters["SMBUS_TIMEOUT_FLG"] = decRng(lPLLReg_11, 5);

	lPLLRegisters["SYSINCAL"] = decRng(lPLLReg_c, 0);
	lPLLRegisters["SYSINCAL_FLG"] = decRng(lPLLReg_11, 0);

	lPLLRegisters["XAXB_ERR"] = decRng(lPLLReg_c, 3);
	lPLLRegisters["XAXB_ERR_FLG"] = decRng(lPLLReg_11, 3);

	lStatus << formatRegTable(lPLLRegisters, "PLL state");

	if (aPrint) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void 
IONode::writeSoftResetRegister() const {
	getNode("csr.ctrl.soft_rst").write(0x1);
	getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
IONode::softReset() const {
	writeSoftResetRegister();
	ERS_INFO("Soft reset done");
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
IONode::getSFPStatus(uint32_t aSFPId, bool aPrint) const {
	std::stringstream lStatus;
	std::string lSFPI2CBus;
	try {
		lSFPI2CBus = mSFPI2CBuses.at(aSFPId);
	} catch(const std::out_of_range& e) {
        throw InvalidSFPId(ERS_HERE, getId(), formatRegValue(aSFPId), e);
	}
	auto sfp = getI2CDevice<I2CSFPSlave>(lSFPI2CBus, "SFP_EEProm");
	lStatus << sfp->get_status();
	if (aPrint) std::cout << lStatus.str();
	return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
IONode::switchSFPSoftTxControlBit(uint32_t aSFPId, bool aOn) const {
	std::string lSFPI2CBus;
	try {
		lSFPI2CBus = mSFPI2CBuses.at(aSFPId);
	} catch(const std::out_of_range& e) {
        throw InvalidSFPId(ERS_HERE, getId(), formatRegValue(aSFPId), e);
	}
	auto sfp = getI2CDevice<I2CSFPSlave>(lSFPI2CBus, "SFP_EEProm");
	sfp->switchSoftTxControlBit(aOn);
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq