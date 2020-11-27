#include "pdt/PC059IONode.hpp"

namespace pdt {

UHAL_REGISTER_DERIVED_NODE(PC059IONode);

//-----------------------------------------------------------------------------
PC059IONode::PC059IONode(const uhal::Node& aNode) :
	FanoutIONode(aNode, "i2c", "FMC_UID_PROM", "i2c", "SI5345", {"PLL", "CDR"}, {"usfp_i2c", "i2c"}) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
PC059IONode::~PC059IONode() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
PC059IONode::getStatus(bool aPrint) const {
	std::stringstream lStatus;
	auto subnodes = readSubNodes(getNode("csr.stat"));
	lStatus << formatRegTable(subnodes, "PC059 IO state");
	
	if (aPrint) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PC059IONode::reset(int32_t aFanoutMode, const std::string& aClockConfigFile) const {
	
	// Soft reset
	writeSoftResetRegister();
	
	millisleep(1000);

	// Reset PLL and I2C
	getNode("csr.ctrl.pll_rst").write(0x1);
	getNode("csr.ctrl.pll_rst").write(0x0);

	getNode("csr.ctrl.rst_i2c").write(0x1);
	getNode("csr.ctrl.rst_i2c").write(0x0);

	getNode("csr.ctrl.rst_i2cmux").write(0x1);
	getNode("csr.ctrl.rst_i2cmux").write(0x0);

	getClient().dispatch();

	// enclustra i2c switch stuff
	try {
		getNode<I2CMasterNode>(mUIDI2CBus).getSlave("AX3_Switch").writeI2C(0x01, 0x7f);
	} catch(...) {
	}

	// Find the right pll config file
	std:: string lClockConfigFile = getFullClockConfigFilePath(aClockConfigFile, aFanoutMode);
	PDT_LOG(kNotice) << "PLL configuration file : " << lClockConfigFile;

	// Upload config file to PLL
	configurePLL(lClockConfigFile);
	
	getNode("csr.ctrl.mux").write(0);
	getClient().dispatch();
	
	auto lSFPExp = getI2CDevice<I2CExpanderSlave>(mUIDI2CBus, "SFPExpander");
	
	// Set invert registers to default for both banks
	lSFPExp->setInversion(0, 0x00);
	lSFPExp->setInversion(1, 0x00);
	
	// Bank 0 input, bank 1 output
	lSFPExp->setIO(0, 0x00);
	lSFPExp->setIO(1, 0xff);
	
	// Bank 0 - enable all SFPGs (enable low)
	lSFPExp->setOutputs(0, 0x00);
    PDT_LOG(kNotice) << "SFPs 0-7 enabled";

	getNode("csr.ctrl.rst_lock_mon").write(0x1);
	getNode("csr.ctrl.rst_lock_mon").write(0x0);
	getClient().dispatch();

	PDT_LOG(kNotice) << "Reset done";
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PC059IONode::reset(const std::string& aClockConfigFile) const {
	reset(-1, aClockConfigFile);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PC059IONode::switchSFPMUXChannel(uint32_t aSFPID) const {
	getNode("csr.ctrl.mux").write(aSFPID);
	getClient().dispatch();
	
	PDT_LOG(kNotice) << "SFP input mux set to " << readActiveSFPMUXChannel();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
PC059IONode::readActiveSFPMUXChannel() const {
	auto lActiveSFPMUXChannel = getNode("csr.ctrl.mux").read();
	getClient().dispatch();
	return lActiveSFPMUXChannel.value();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PC059IONode::switchSFPI2CMUXChannel(uint32_t aSFPId) const {

	getNode("csr.ctrl.rst_i2cmux").write(0x1);
	getClient().dispatch();
	getNode("csr.ctrl.rst_i2cmux").write(0x0);
	getClient().dispatch();
	millisleep(100);
	
	
	uint8_t lChannelSelectByte = 1UL << aSFPId;
	getNode<I2CMasterNode>(mPLLI2CBus).getSlave("SFP_Switch").writeI2CPrimitive({lChannelSelectByte});
	PDT_LOG(kInfo) << "PC059 SFP I2C mux set to 0x" << std::hex << aSFPId;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
PC059IONode::getSFPStatus(uint32_t aSFPId, bool aPrint) const {
	// on this board the upstream sfp has its own i2c bus, and the 8 downstream sfps are muxed onto the main i2c bus
	std::stringstream lStatus;
	uint32_t lSFPBusId;
	if (aSFPId == 0) {
		lSFPBusId = 0;
		lStatus << "Upstream SFP:" << std::endl;
	} else if (aSFPId > 0 && aSFPId < 9) {
		switchSFPI2CMUXChannel(aSFPId-1);
		lStatus << "Fanout SFP " << aSFPId-1 << ":" << std::endl;
		lSFPBusId = 1;
	} else {
		std::ostringstream lMsg;
        lMsg << "Invalid SFP ID: " << aSFPId << ". PC059 valid range: 0-8";
        throw InvalidSFPId(lMsg.str());
	}
	auto sfp = getI2CDevice<I2CSFPSlave>(mSFPI2CBuses.at(lSFPBusId), "SFP_EEProm");
	
	lStatus << sfp->getStatus();	
	
	if (aPrint) std::cout << lStatus.str();
	return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PC059IONode::switchSFPSoftTxControlBit(uint32_t aSFPId, bool aOn) const {
	// on this board the upstream sfp has its own i2c bus, and the 8 downstream sfps are muxed onto the main i2c bus
	uint32_t lSFPBusId;
	if (aSFPId == 0) {
		lSFPBusId = 0;
	} else if (aSFPId > 0 && aSFPId < 9) {
		switchSFPI2CMUXChannel(aSFPId-1);
		lSFPBusId = 1;
	} else {
		std::ostringstream lMsg;
        lMsg << "Invalid SFP ID: " << aSFPId << ". PC059 valid range: 0-8";
        throw InvalidSFPId(lMsg.str());
	}
	auto sfp = getI2CDevice<I2CSFPSlave>(mSFPI2CBuses.at(lSFPBusId), "SFP_EEProm");
	sfp->switchSoftTxControlBit(aOn);
}
//-----------------------------------------------------------------------------
} // namespace pdt