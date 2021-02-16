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
	//mPLL (new SI534xSlave( getNode<I2CMasterNode>(mPLLI2CBus)& , getNode<I2CMasterNode>(mPLLI2CBus).get_slave_address(aPLLI2CDevice) )) 
	{
} 
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
IONode::~IONode() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
IONode::read_board_type() const {
	uhal::ValWord<uint32_t> lBoardType = getNode("config.board_type").read();
    getClient().dispatch();
    return lBoardType.value();  
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
IONode::read_carrier_type() const {
	uhal::ValWord<uint32_t> lCarrierType = getNode("config.carrier_type").read();
    getClient().dispatch();
    return lCarrierType.value();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
IONode::read_design_type() const {
	uhal::ValWord<uint32_t> lDesignType = getNode("config.design_type").read();
	getClient().dispatch();
	return lDesignType.value();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint64_t
IONode::read_board_uid() const {

	uint64_t lUID = 0;
	std::vector<uint8_t> lUIDValues = getNode<I2CMasterNode>(mUIDI2CBus).get_slave(mUIDI2CDevice).read_i2cArray(0xfa, 6);
	
	for (uint8_t i=0; i < lUIDValues.size(); ++i) {
		lUID = (lUID << 8) | lUIDValues.at(i);
	}
	return lUID;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
BoardRevision
IONode::get_board_revision() const {
	
	auto lUID = read_board_uid();
	try {
		return kBoardUIDRevisionMap.at(lUID);
	} catch(const std::out_of_range& e) {
        throw UnknownBoardUID(ERS_HERE, getId(), format_reg_value(lUID), e);
	}
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
IONode::get_hardware_info(bool aPrint) const {
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

	lHardwareInfo.push_back(std::make_pair("Board UID", format_reg_value(read_board_uid())));

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
std::string 
IONode::get_full_clock_config_file_path(const std::string& aClockConfigFile, int32_t aMode) const {

	
	if (aClockConfigFile.size()) {
		// config file provided explicitly, no need for lookup
		return aClockConfigFile;
	} else {

		std::string lConfigFile;
		std::string lClockConfigKey;

		const BoardRevision lBoardRevision = get_board_revision();
		const CarrierType lCarrierType = convert_value_to_carrier_type(read_carrier_type());
		const DesignType lDesignType = convert_value_to_design_type(read_design_type());

		try {
			lClockConfigKey = kBoardRevisionMap.at(lBoardRevision) + "_";
		} catch(const std::out_of_range& e) {
        	throw MissingBoardRevisionMapEntry(ERS_HERE, getId(), format_reg_value(lBoardRevision), e);
		}

		try {
			lClockConfigKey = lClockConfigKey + kCarrierTypeMap.at(lCarrierType) + "_";
		} catch(const std::out_of_range& e) {
        	throw MissingCarrierTypeMapEntry(ERS_HERE, getId(), format_reg_value(lCarrierType), e);
		}

		try {
			lClockConfigKey = lClockConfigKey + kDesignTypeMap.at(lDesignType);
		} catch(const std::out_of_range& e) {
        	throw MissingDesignTypeMapEntry(ERS_HERE, getId(), format_reg_value(lDesignType), e);
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
IONode::get_pll() const {
	return get_i2c_device<SI534xSlave>(mPLLI2CBus, mPLLI2CDevice);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void 
IONode::configure_pll(const std::string& aClockConfigFile) const {
	auto pll = get_pll();

	uint32_t lSIVersion = pll->read_device_version();
	ERS_INFO("Configuring PLL        : SI" << format_reg_value(lSIVersion));

	pll->configure(aClockConfigFile);
	
	ERS_INFO("PLL configuration id   : " << pll->read_config_id());
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<double>
IONode::read_clock_frequencies() const {
	return getNode<FrequencyCounterNode>("freq").measure_frequencies(mClockNames.size());
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
IONode::get_clock_frequencies_table(bool aPrint) const {
	std::stringstream lTable;
	std::vector<double> lFrequencies = read_clock_frequencies();
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
IONode::get_pll_status(bool aPrint) const {

	std::stringstream lStatus;

	auto pll = get_pll();
	lStatus << "PLL configuration id   : " << pll->read_config_id() << std::endl;

	std::map<std::string, uint32_t> lPLLVersion;
	lPLLVersion["Part number"] = pll->read_device_version();
	lPLLVersion["Device grade"] = pll->read_clock_register(0x4);
	lPLLVersion["Device revision"] = pll->read_clock_register(0x5);
	
	lStatus << format_reg_table(lPLLVersion, "PLL information") << std::endl;

	std::map<std::string, uint32_t> lPLLRegisters;

	uint8_t lPLLReg_c = pll->read_clock_register(0xc);
	uint8_t lPLLReg_d = pll->read_clock_register(0xd);
	uint8_t lPLLReg_e = pll->read_clock_register(0xe);
	uint8_t lPLLReg_f = pll->read_clock_register(0xf);
	uint8_t lPLLReg_11 = pll->read_clock_register(0x11);
	uint8_t lPLLReg_12 = pll->read_clock_register(0x12);

	lPLLRegisters["CAL_PLL"] = dec_rng(lPLLReg_f, 5);
	lPLLRegisters["HOLD"] = dec_rng(lPLLReg_e, 5);
	lPLLRegisters["LOL"] = dec_rng(lPLLReg_e, 1);
	lPLLRegisters["LOS"] = dec_rng(lPLLReg_d, 0, 4);
	lPLLRegisters["LOSXAXB"] = dec_rng(lPLLReg_c, 1);
	lPLLRegisters["LOSXAXB_FLG"] = dec_rng(lPLLReg_11, 1);

	lPLLRegisters["OOF"] = dec_rng(lPLLReg_d, 4, 4);
    lPLLRegisters["OOF (sticky)"] = dec_rng(lPLLReg_12, 4, 4);

	lPLLRegisters["SMBUS_TIMEOUT"] = dec_rng(lPLLReg_c, 5);
	lPLLRegisters["SMBUS_TIMEOUT_FLG"] = dec_rng(lPLLReg_11, 5);

	lPLLRegisters["SYSINCAL"] = dec_rng(lPLLReg_c, 0);
	lPLLRegisters["SYSINCAL_FLG"] = dec_rng(lPLLReg_11, 0);

	lPLLRegisters["XAXB_ERR"] = dec_rng(lPLLReg_c, 3);
	lPLLRegisters["XAXB_ERR_FLG"] = dec_rng(lPLLReg_11, 3);

	lStatus << format_reg_table(lPLLRegisters, "PLL state");

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
IONode::soft_reset() const {
	writeSoftResetRegister();
	ERS_INFO("Soft reset done");
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
IONode::get_sfp_status(uint32_t aSFPId, bool aPrint) const {
	std::stringstream lStatus;
	std::string lSFPI2CBus;
	try {
		lSFPI2CBus = mSFPI2CBuses.at(aSFPId);
	} catch(const std::out_of_range& e) {
        throw InvalidSFPId(ERS_HERE, getId(), format_reg_value(aSFPId), e);
	}
	auto sfp = get_i2c_device<I2CSFPSlave>(lSFPI2CBus, "SFP_EEProm");
	lStatus << sfp->get_status();
	if (aPrint) std::cout << lStatus.str();
	return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
IONode::switch_sfp_soft_tx_control_bit(uint32_t aSFPId, bool aOn) const {
	std::string lSFPI2CBus;
	try {
		lSFPI2CBus = mSFPI2CBuses.at(aSFPId);
	} catch(const std::out_of_range& e) {
        throw InvalidSFPId(ERS_HERE, getId(), format_reg_value(aSFPId), e);
	}
	auto sfp = get_i2c_device<I2CSFPSlave>(lSFPI2CBus, "SFP_EEProm");
	sfp->switch_soft_tx_control_bit(aOn);
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq