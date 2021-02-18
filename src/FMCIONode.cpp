#include "pdt/FMCIONode.hpp"

namespace dunedaq {
namespace pdt {

UHAL_REGISTER_DERIVED_NODE(FMCIONode)

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
FMCIONode::get_status(bool aPrint) const {
	std::stringstream lStatus;

	auto subnodes = read_sub_nodes(getNode("csr.stat"));
	lStatus << format_reg_table(subnodes, "FMC IO state");

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

	CarrierType lCarrierType = convert_value_to_carrier_type(read_carrier_type());

	// enclustra i2c switch stuff
	if (lCarrierType == kCarrierEnclustraA35) {
		try {
			getNode<I2CMasterNode>(mUIDI2CBus).get_slave("AX3_Switch").write_i2c(0x01, 0x7f);
		} catch(...) {
		}
	}

	// Find the right pll config file
	std:: string lClockConfigFile = get_full_clock_config_file_path(aClockConfigFile);
	ERS_INFO("PLL configuration file : " << lClockConfigFile);

	// Upload config file to PLL
	configure_pll(lClockConfigFile);
	
	// Disable sfp tx laser	
	getNode("csr.ctrl.sfp_tx_dis").write(0x0);

	getNode("csr.ctrl.rst_lock_mon").write(0x1);
	getNode("csr.ctrl.rst_lock_mon").write(0x0);
	getClient().dispatch();
	
	ERS_INFO("Reset done");
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FMCIONode::get_info(timingmon::TimingFMCMonitorData& mon_data) const {

	auto subnodes = read_sub_nodes(getNode("csr.stat"));

	mon_data.cdr_lol = subnodes.at("cdr_lol").value();
	mon_data.cdr_los = subnodes.at("cdr_los").value();
	mon_data.mmcm_ok = subnodes.at("mmcm_ok").value();
	mon_data.mmcm_sticky = subnodes.at("mmcm_sticky").value();
	mon_data.sfp_flt = subnodes.at("sfp_flt").value();
	mon_data.sfp_los = subnodes.at("sfp_los").value();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FMCIONode::get_info(timingmon::TimingFMCMonitorDataDebug& mon_data) const {

	// TODO, do something clever with function above?
	auto subnodes = read_sub_nodes(getNode("csr.stat"));

	mon_data.cdr_lol = subnodes.at("cdr_lol");
	mon_data.cdr_los = subnodes.at("cdr_los");
	mon_data.mmcm_ok = subnodes.at("mmcm_ok");
	mon_data.mmcm_sticky = subnodes.at("mmcm_sticky");
	mon_data.sfp_flt = subnodes.at("sfp_flt");
	mon_data.sfp_los = subnodes.at("sfp_los");

	this->get_pll()->get_info(mon_data.pll_mon_data);
	auto sfp = this->get_i2c_device<I2CSFPSlave>(mSFPI2CBuses.at(0), "SFP_EEProm");
	sfp->get_info(mon_data.sfp_mon_data);
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq