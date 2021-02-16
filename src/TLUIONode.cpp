#include "pdt/TLUIONode.hpp"

namespace dunedaq {
namespace pdt {

UHAL_REGISTER_DERIVED_NODE(TLUIONode)

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
TLUIONode::get_status(bool aPrint) const {
	auto subnodes = read_sub_nodes(getNode("csr.stat"));
	std::stringstream lStatus;
	lStatus << format_reg_table(subnodes, "TLU IO state");

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
		getNode<I2CMasterNode>(mUIDI2CBus).get_slave("AX3_Switch").write_i2c(0x01, 0x7f);
	} catch(...) {
	}

	// Find the right pll config file
	std:: string lClockConfigFile = get_full_clock_config_file_path(aClockConfigFile);
	ERS_INFO("PLL configuration file : " << lClockConfigFile);
	
	// Upload config file to PLL
	configure_pll(lClockConfigFile);

	// Tweak the PLL swing
	auto lSIChip = get_pll();
	lSIChip->write_i2cArray(0x113, {0x9, 0x33});

	// configure tlu io expanders
	auto lIC6 = get_i2c_device<I2CExpanderSlave>(mUIDI2CBus, "Expander1");
	auto lIC7 = get_i2c_device<I2CExpanderSlave>(mUIDI2CBus, "Expander2");

	// Bank 0
	lIC6->set_inversion(0, 0x00);
	lIC6->set_io(0, 0x00);
	lIC6->set_outputs(0, 0x00);

	// Bank 1
	lIC6->set_inversion(1, 0x00);
	lIC6->set_io(1, 0x00);
	lIC6->set_outputs(1, 0x88);

	// Bank 0
	lIC7->set_inversion(0, 0x00);
	lIC7->set_io(0, 0x00);
	lIC7->set_outputs(0, 0xf0);

	// Bank 1
	lIC7->set_inversion(1, 0x00);
	lIC7->set_io(1, 0x00);
	lIC7->set_outputs(1, 0xf0);

	// BI signals are NIM
	uint32_t lBISignalThreshold = 0x589D;
	
	configure_dac(0, lBISignalThreshold);
	configure_dac(1, lBISignalThreshold);

	ERS_INFO("DAC1 and DAC2 set to " << std::hex << lBISignalThreshold);

	getNode("csr.ctrl.rst_lock_mon").write(0x1);
	getNode("csr.ctrl.rst_lock_mon").write(0x0);
	getClient().dispatch();

	ERS_INFO("Reset done");
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
TLUIONode::configure_dac(uint32_t aDACId, uint32_t aDACValue, bool aInternalRef) const {
	std::string lDACDevice;
	try {
		lDACDevice = mDACDevices.at(aDACId);
	} catch(const std::out_of_range& e) {
        throw InvalidDACId(ERS_HERE, getId(), format_reg_value(aDACId));
	}
	auto lDAC = get_i2c_device<DACSlave>(mUIDI2CBus, lDACDevice);
	lDAC->setInteralRef(aInternalRef);
    lDAC->setDAC(7, aDACValue);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
TLUIONode::get_sfp_status(uint32_t /*aSFPId*/, bool /*aPrint*/) const {
	ERS_LOG("TLU does not support SFP I2C");
	return "";
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
TLUIONode::switch_sfp_soft_tx_control_bit(uint32_t /*aSFPId*/, bool /*aOn*/) const {
	ERS_LOG("TLU does not support SFP I2C");
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
TLUIONode::get_info(timing::timingmon::TimingTLUMonitorData& mon_data) const {

	auto subnodes = read_sub_nodes(getNode("csr.stat"));

	mon_data.cdr_lol = subnodes.at("cdr_lol");
	mon_data.cdr_los = subnodes.at("cdr_los");
	mon_data.mmcm_ok = subnodes.at("mmcm_ok");
	mon_data.mmcm_sticky = subnodes.at("mmcm_sticky");
	mon_data.pll_ok = subnodes.at("pll_ok");
	mon_data.pll_sticky = subnodes.at("pll_sticky");
	mon_data.sfp_flt = subnodes.at("sfp_fault");
	mon_data.sfp_los = subnodes.at("sfp_los");
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq