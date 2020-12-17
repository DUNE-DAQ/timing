#include "pdt/FIBIONode.hpp"

namespace pdt {

UHAL_REGISTER_DERIVED_NODE(FIBIONode);

//-----------------------------------------------------------------------------
FIBIONode::FIBIONode(const uhal::Node& aNode) :
	FanoutIONode(aNode, "i2c", "FMC_UID_PROM", "i2c", "SI5345", {"PLL", "CDR"}, {"usfp_i2c", "i2c"}) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
FIBIONode::~FIBIONode() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
FIBIONode::getStatus(bool aPrint) const {
	std::stringstream lStatus;
	std::map<std::string,uhal::ValWord<uint32_t>> subnodes = readSubNodes(getNode("csr.stat"));

	uhal::ValWord<uint32_t> lSFPLOSFlags(readSFPLOSFlags());
	lSFPLOSFlags.valid(true);
	subnodes.emplace("SFP LOS flags", lSFPLOSFlags);
	
	uhal::ValWord<uint32_t> lSFPFaultFlags(readSFPFaultFlags());
	lSFPFaultFlags.valid(true);
	subnodes.emplace("SFP Fault flags", lSFPFaultFlags);

	lStatus << formatRegTable(subnodes, "FIB IO state");

	if (aPrint) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBIONode::reset(int32_t aFanoutMode, const std::string& aClockConfigFile) const {
	
	// Soft reset
	writeSoftResetRegister();
	
	millisleep(1000);

	// Reset I2C
	getNode("csr.ctrl.rstb_i2c").write(0x1);
	getNode("csr.ctrl.rstb_i2c").write(0x0);
	getClient().dispatch();

	const CarrierType lCarrierType = convertValueToCarrierType(readCarrierType());

	if (lCarrierType == kCarrierEnclustraA35) {
		// enclustra i2c switch stuff
		try {
			getNode<I2CMasterNode>(mUIDI2CBus).getSlave("AX3_Switch").writeI2C(0x01, 0x7f);
		} catch(...) {
		}
	}
	
	// Configure I2C IO expanders
	auto lIC10 = getI2CDevice<I2CExpanderSlave>(mUIDI2CBus, "Expander1");
	auto lIC23 = getI2CDevice<I2CExpanderSlave>(mUIDI2CBus, "Expander2");

	// Bank 0
	lIC10->setInversion(0, 0x00);
	
	//  all out, sfp tx disable
	lIC10->setIO(0, 0x00);
	// sfp laser on by default
	lIC10->setOutputs(0, 0x00);

	// Bank 1
	lIC10->setInversion(1, 0x00);
	// all inputs, sfp fault
	lIC10->setIO(1, 0xff);

	// Bank 0
	lIC23->setInversion(0, 0x00);
	// pin 0 - out: pll rst, pins 1-4 pll and cdr flags
	lIC23->setIO(0, 0xfe);
	lIC23->setOutputs(0, 0x01);

	// Bank 1
	lIC23->setInversion(1, 0x00);
	// all inputs, sfp los
	lIC23->setIO(1, 0xff);

	// reset pll via I2C IO expanders
	resetPLL();
	
	// Find the right pll config file
	std:: string lClockConfigFile = getFullClockConfigFilePath(aClockConfigFile, aFanoutMode);
	PDT_LOG(kNotice) << "PLL configuration file : " << lClockConfigFile;

	// Upload config file to PLL
	configurePLL(lClockConfigFile);
	
	//getNode("csr.ctrl.inmux").write(0);
	//getClient().dispatch();
	
//	getNode("csr.ctrl.rst_lock_mon").write(0x1);
//	getNode("csr.ctrl.rst_lock_mon").write(0x0);
//	getClient().dispatch();

	PDT_LOG(kNotice) << "Reset done";
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBIONode::reset(const std::string& aClockConfigFile) const {
	reset(-1, aClockConfigFile);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBIONode::switchSFPMUXChannel(uint32_t aSFPId) const {
	validateSFPId(aSFPId);
	getNode("csr.ctrl.inmux").write(aSFPId);
	getClient().dispatch();	
	PDT_LOG(kNotice) << "SFP input mux set to " << readActiveSFPMUXChannel();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
FIBIONode::readActiveSFPMUXChannel() const {
	auto lActiveSFPMUXChannel = getNode("csr.ctrl.inmux").read();
	getClient().dispatch();
	return lActiveSFPMUXChannel.value();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
FIBIONode::getSFPStatus(uint32_t aSFPId, bool aPrint) const {
	std::stringstream lStatus;
	
	validateSFPId(aSFPId);

	std::string lSFPI2CBus = "i2c_sfp" + std::to_string(aSFPId);
	auto sfp = getI2CDevice<I2CSFPSlave>(lSFPI2CBus, "SFP_EEProm");
	lStatus << "Fanout SFP " << aSFPId << ":" << std::endl;
	lStatus << sfp->getStatus();	
	
	if (aPrint) std::cout << lStatus.str();
	return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBIONode::switchSFPSoftTxControl(uint32_t aSFPId, bool aOn) const {
	validateSFPId(aSFPId);

	// on this board the 8 downstream sfps have their own i2c bus
	std::string lSFPI2CBus = "i2c_sfp" + std::to_string(aSFPId);
	auto sfp = getI2CDevice<I2CSFPSlave>(lSFPI2CBus, "SFP_EEProm");
	sfp->switchSoftTxControl(aOn);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBIONode::resetPLL() const {
	auto lIC23 = getI2CDevice<I2CExpanderSlave>(mUIDI2CBus, "Expander2");
	lIC23->setOutputs(0, 0x00);
	lIC23->setOutputs(0, 0x01);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint8_t
FIBIONode::readSFPLOSFlags() const {
	auto lIC23 = getI2CDevice<I2CExpanderSlave>(mUIDI2CBus, "Expander2");
	
	uint8_t lSFPLOSFlags = lIC23->readInputs(0x01);
	return lSFPLOSFlags;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint8_t
FIBIONode::readSFPFaultFlags() const {
	auto lIC10 = getI2CDevice<I2CExpanderSlave>(mUIDI2CBus, "Expander1");
	
	uint8_t lSFPFaultFlags = lIC10->readInputs(0x01);
	return lSFPFaultFlags;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint8_t
FIBIONode::readSFPLOSFlag(uint32_t aSFPId) const {
	return readSFPLOSFlags() & (1UL << aSFPId);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint8_t
FIBIONode::readSFPFaultFlag(uint32_t aSFPId) const {
	return readSFPFaultFlags() & (1UL << aSFPId);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBIONode::switchSFPTx(uint32_t aSFPId, bool aOn) const {
	validateSFPId(aSFPId);
	
	auto lIC10 = getI2CDevice<I2CExpanderSlave>(mUIDI2CBus, "Expander1");
	uint8_t lCurrentSFPTxControlFlags = lIC10->readOutputsConfig(0);

	uint8_t lNewSFPTxControlFlags;
	if (aOn) {
		lNewSFPTxControlFlags = lCurrentSFPTxControlFlags & ~(1UL << aSFPId);
	}
    else {
    	lNewSFPTxControlFlags = lCurrentSFPTxControlFlags | (1UL << aSFPId);
    }

    lIC10->setOutputs(0, lNewSFPTxControlFlags);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FIBIONode::validateSFPId(uint32_t aSFPId) const {
	// on this board we have 8 downstream SFPs
	if (aSFPId > 7) {
		std::ostringstream lMsg;
        lMsg << "Invalid SFP ID: " << aSFPId << ". FIB valid range: 0-7";
        throw InvalidSFPId(lMsg.str());
	}
}
//-----------------------------------------------------------------------------
} // namespace pdt