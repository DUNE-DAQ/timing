#include "timing/FIBIONode.hpp"

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(FIBIONode)

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
FIBIONode::get_status(bool print_out) const {
	std::stringstream lStatus;
	std::map<std::string,uhal::ValWord<uint32_t>> subnodes = read_sub_nodes(getNode("csr.stat"));

	uhal::ValWord<uint32_t> lSFPLOSFlags(read_sfp_los_flags());
	lSFPLOSFlags.valid(true);
	subnodes.emplace("SFP LOS flags", lSFPLOSFlags);
	
	uhal::ValWord<uint32_t> lSFPFaultFlags(read_sfp_fault_flags());
	lSFPFaultFlags.valid(true);
	subnodes.emplace("SFP Fault flags", lSFPFaultFlags);

	lStatus << format_reg_table(subnodes, "FIB IO state");

	if (print_out) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBIONode::reset(int32_t fanout_mode, const std::string& clock_config_file) const {
	
	// Soft reset
	write_soft_reset_register();
	
	millisleep(1000);

	// Reset I2C
	getNode("csr.ctrl.rstb_i2c").write(0x1);
	getNode("csr.ctrl.rstb_i2c").write(0x0);
	getClient().dispatch();

	const CarrierType lCarrierType = convert_value_to_carrier_type(read_carrier_type());

	if (lCarrierType == kCarrierEnclustraA35) {
		// enclustra i2c switch stuff
		try {
			getNode<I2CMasterNode>(m_uid_i2c_bus).get_slave("AX3_Switch").write_i2c(0x01, 0x7f);
		} catch(...) {
		}
	}
	
	// Configure I2C IO expanders
	auto lIC10 = get_i2c_device<I2CExpanderSlave>(m_uid_i2c_bus, "Expander1");
	auto lIC23 = get_i2c_device<I2CExpanderSlave>(m_uid_i2c_bus, "Expander2");

	// Bank 0
	lIC10->set_inversion(0, 0x00);
	
	//  all out, sfp tx disable
	lIC10->set_io(0, 0x00);
	// sfp laser on by default
	lIC10->set_outputs(0, 0x00);

	// Bank 1
	lIC10->set_inversion(1, 0x00);
	// all inputs, sfp fault
	lIC10->set_io(1, 0xff);

	// Bank 0
	lIC23->set_inversion(0, 0x00);
	// pin 0 - out: pll rst, pins 1-4 pll and cdr flags
	lIC23->set_io(0, 0xfe);
	lIC23->set_outputs(0, 0x01);

	// Bank 1
	lIC23->set_inversion(1, 0x00);
	// all inputs, sfp los
	lIC23->set_io(1, 0xff);

	// reset pll via I2C IO expanders
	reset_pll();
	
	// Find the right pll config file
	std:: string lClockConfigFile = get_full_clock_config_file_path(clock_config_file, fanout_mode);
	TLOG_DEBUG(0) << "PLL configuration file : " << lClockConfigFile;

	// Upload config file to PLL
	configure_pll(lClockConfigFile);
	
	//getNode("csr.ctrl.inmux").write(0);
	//getClient().dispatch();
	
//	getNode("csr.ctrl.rst_lock_mon").write(0x1);
//	getNode("csr.ctrl.rst_lock_mon").write(0x0);
//	getClient().dispatch();

	TLOG() << "Reset done";
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBIONode::reset(const std::string& clock_config_file) const {
	reset(-1, clock_config_file);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBIONode::switch_sfp_mux_channel(uint32_t sfp_id) const {
	validate_sfp_id(sfp_id);
	getNode("csr.ctrl.inmux").write(sfp_id);
	getClient().dispatch();	
	TLOG_DEBUG(0) << "SFP input mux set to " << read_active_sfp_mux_channel();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
FIBIONode::read_active_sfp_mux_channel() const {
	auto lActiveSFPMUXChannel = getNode("csr.ctrl.inmux").read();
	getClient().dispatch();
	return lActiveSFPMUXChannel.value();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
FIBIONode::get_sfp_status(uint32_t sfp_id, bool print_out) const {
	std::stringstream lStatus;
	
	validate_sfp_id(sfp_id);

	std::string lSFPI2CBus = "i2c_sfp" + std::to_string(sfp_id);
	auto sfp = get_i2c_device<I2CSFPSlave>(lSFPI2CBus, "SFP_EEProm");
	lStatus << "Fanout SFP " << sfp_id << ":" << std::endl;
	lStatus << sfp->get_status();	
	
	if (print_out) std::cout << lStatus.str();
	return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBIONode::switch_sfp_soft_tx_control_bit(uint32_t sfp_id, bool turn_on) const {
	validate_sfp_id(sfp_id);

	// on this board the 8 downstream sfps have their own i2c bus
	std::string lSFPI2CBus = "i2c_sfp" + std::to_string(sfp_id);
	auto sfp = get_i2c_device<I2CSFPSlave>(lSFPI2CBus, "SFP_EEProm");
	sfp->switch_soft_tx_control_bit(turn_on);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBIONode::reset_pll() const {
	auto lIC23 = get_i2c_device<I2CExpanderSlave>(m_uid_i2c_bus, "Expander2");
	lIC23->set_outputs(0, 0x00);
	lIC23->set_outputs(0, 0x01);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint8_t
FIBIONode::read_sfp_los_flags() const {
	auto lIC23 = get_i2c_device<I2CExpanderSlave>(m_uid_i2c_bus, "Expander2");
	
	uint8_t lSFPLOSFlags = lIC23->read_inputs(0x01);
	return lSFPLOSFlags;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint8_t
FIBIONode::read_sfp_fault_flags() const {
	auto lIC10 = get_i2c_device<I2CExpanderSlave>(m_uid_i2c_bus, "Expander1");
	
	uint8_t lSFPFaultFlags = lIC10->read_inputs(0x01);
	return lSFPFaultFlags;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint8_t
FIBIONode::read_sfp_los_flag(uint32_t sfp_id) const {
	return read_sfp_los_flags() & (1UL << sfp_id);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint8_t
FIBIONode::read_sfp_fault_flag(uint32_t sfp_id) const {
	return read_sfp_fault_flags() & (1UL << sfp_id);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBIONode::switch_sfp_tx(uint32_t sfp_id, bool turn_on) const {
	validate_sfp_id(sfp_id);
	
	auto lIC10 = get_i2c_device<I2CExpanderSlave>(m_uid_i2c_bus, "Expander1");
	uint8_t lCurrentSFPTxControlFlags = lIC10->read_outputs_config(0);

	uint8_t lNewSFPTxControlFlags;
	if (turn_on) {
		lNewSFPTxControlFlags = lCurrentSFPTxControlFlags & ~(1UL << sfp_id);
	}
    else {
    	lNewSFPTxControlFlags = lCurrentSFPTxControlFlags | (1UL << sfp_id);
    }

    lIC10->set_outputs(0, lNewSFPTxControlFlags);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBIONode::validate_sfp_id(uint32_t sfp_id) const {
	// on this board we have 8 downstream SFPs
	if (sfp_id > 7) {
		std::ostringstream lMsg;
        lMsg << "Invalid SFP ID: " << sfp_id << ". FIB valid range: 0-7";
        throw InvalidSFPId(ERS_HERE, format_reg_value(sfp_id));
	}
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq