#include "pdt/TLUIONode.hpp"

namespace pdt {

UHAL_REGISTER_DERIVED_NODE(TLUIONode);

//-----------------------------------------------------------------------------
TLUIONode::TLUIONode(const uhal::Node& aNode) :
	IONode(aNode, "i2c", "UID_PROM", "i2c", "SI5345", {"PLL"}, {}),
	mDACDevices({"DAC1", "DAC2"}) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
TLUIONode::~TLUIONode() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
TLUIONode::getStatus(bool aPrint) const {
	auto subnodes = readSubNodes(getNode("csr.stat"));
	std::stringstream lStatus;
	lStatus << formatRegTable(subnodes, "TLU IO state");

	if (aPrint) std::cout << lStatus.str();
	return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
TLUIONode::reset(const std::string& aClockConfigFile) const {
	// Soft reset
	writeSoftResetRegister();
	
	millisleep(1000);

	// Reset PLL and I2C
	getNode("csr.ctrl.pll_rst").write(0x1);
	getNode("csr.ctrl.pll_rst").write(0x0);

	getNode("csr.ctrl.rst_i2c").write(0x1);
	getNode("csr.ctrl.rst_i2c").write(0x0);

	getClient().dispatch();

	// enclustra i2c switch stuff
	try {
		getNode<I2CMasterNode>(mUIDI2CBus).getSlave("AX3_Switch").writeI2C(0x01, 0x7f);
	} catch(...) {
	}

	// Find the right pll config file
	std:: string lClockConfigFile = getFullClockConfigFilePath(aClockConfigFile);
	PDT_LOG(kNotice) << "PLL configuration file : " << lClockConfigFile;
	
	// Upload config file to PLL
	configurePLL(lClockConfigFile);

	// Tweak the PLL swing
	auto lSIChip = getPLL();
	lSIChip->writeI2CArray(0x113, {0x9, 0x33});

	// configure tlu io expanders
	auto lIC6 = getI2CDevice<I2CExpanderSlave>(mUIDI2CBus, "Expander1");
	auto lIC7 = getI2CDevice<I2CExpanderSlave>(mUIDI2CBus, "Expander2");

	// Bank 0
	lIC6->setInversion(0, 0x00);
	lIC6->setIO(0, 0x00);
	lIC6->setOutputs(0, 0x00);

	// Bank 1
	lIC6->setInversion(1, 0x00);
	lIC6->setIO(1, 0x00);
	lIC6->setOutputs(1, 0x88);

	// Bank 0
	lIC7->setInversion(0, 0x00);
	lIC7->setIO(0, 0x00);
	lIC7->setOutputs(0, 0xf0);

	// Bank 1
	lIC7->setInversion(1, 0x00);
	lIC7->setIO(1, 0x00);
	lIC7->setOutputs(1, 0xf0);

	// BI signals are NIM
	uint32_t lBISignalThreshold = 0x589D;
	
	configureDAC(0, lBISignalThreshold);
	configureDAC(1, lBISignalThreshold);

	PDT_LOG(kNotice) << "DAC1 and DAC2 set to " << std::hex << lBISignalThreshold;

	getNode("csr.ctrl.rst_lock_mon").write(0x1);
	getNode("csr.ctrl.rst_lock_mon").write(0x0);
	getClient().dispatch();

	PDT_LOG(kNotice) << "Reset done";
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
TLUIONode::configureDAC(uint32_t aDACId, uint32_t aDACValue, bool aInternalRef) const {
	std::string lDACDevice;
	try {
		lDACDevice = mDACDevices.at(aDACId);
	} catch(const std::out_of_range& e) {
		std::ostringstream lMsg;
        lMsg << "Invalid DAC ID: " << aDACId;
        throw InvalidDACId(lMsg.str());
	}
	auto lDAC = getI2CDevice<DACSlave>(mUIDI2CBus, lDACDevice);
	lDAC->setInteralRef(aInternalRef);
    lDAC->setDAC(7, aDACValue);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
TLUIONode::getSFPStatus(uint32_t aSFPId, bool aPrint) const {
	PDT_LOG(kWarning) << "TLU does not support SFP I2C";
	return "";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
TLUIONode::switchSFPSoftTxControlBit(uint32_t aSFPId, bool aOn) const {
	PDT_LOG(kWarning) << "TLU does not support SFP I2C";
}
//-----------------------------------------------------------------------------
} // namespace pdt