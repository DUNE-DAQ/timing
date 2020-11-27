#include "pdt/FMCIONode.hpp"

namespace pdt {

UHAL_REGISTER_DERIVED_NODE(FMCIONode);

//-----------------------------------------------------------------------------
FMCIONode::FMCIONode(const uhal::Node& aNode) : 
IONode(aNode, "uid_i2c", "FMC_UID_PROM", "pll_i2c", "i2caddr", {"PLL", "CDR"}, {"sfp_i2c"}) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
FMCIONode::~FMCIONode() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string 
FMCIONode::getStatus(bool aPrint) const {
	std::stringstream lStatus;

	auto subnodes = readSubNodes(getNode("csr.stat"));
	lStatus << formatRegTable(subnodes, "FMC IO state");

	if (aPrint) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FMCIONode::reset(const std::string& aClockConfigFile) const {

	writeSoftResetRegister();
	
	millisleep(1000);

	// Reset PLL
	getNode("csr.ctrl.pll_rst").write(0x1);
	getNode("csr.ctrl.pll_rst").write(0x0);
	getClient().dispatch();

	CarrierType lCarrierType = convertValueToCarrierType(readCarrierType());

	// enclustra i2c switch stuff
	if (lCarrierType == kCarrierEnclustraA35) {
		try {
			getNode<I2CMasterNode>(mUIDI2CBus).getSlave("AX3_Switch").writeI2C(0x01, 0x7f);
		} catch(...) {
		}
	}

	// Find the right pll config file
	std:: string lClockConfigFile = getFullClockConfigFilePath(aClockConfigFile);
	PDT_LOG(kNotice) << "PLL configuration file : " << lClockConfigFile;

	// Upload config file to PLL
	configurePLL(lClockConfigFile);
	
	// Disable sfp tx laser	
	getNode("csr.ctrl.sfp_tx_dis").write(0x0);

	getNode("csr.ctrl.rst_lock_mon").write(0x1);
	getNode("csr.ctrl.rst_lock_mon").write(0x0);
	getClient().dispatch();
	
	PDT_LOG(kInfo) << "Reset done";
}
//-----------------------------------------------------------------------------

} // namespace pdt