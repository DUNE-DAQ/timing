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

} // namespace pdt
} // namespace dunedaq