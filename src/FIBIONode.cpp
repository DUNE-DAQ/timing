/**
 * @file FIBIONode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/FIBIONode.hpp"

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(FIBIONode)

//-----------------------------------------------------------------------------
FIBIONode::FIBIONode(const uhal::Node& aNode) :
	SFPMuxIONode(aNode, "i2c", "i2c", "SI5345", {"PLL", "BKP DATA"}, {"i2c_sfp0", "i2c_sfp1", "i2c_sfp2", "i2c_sfp3", "i2c_sfp4", "i2c_sfp5", "i2c_sfp6", "i2c_sfp7"}) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
FIBIONode::~FIBIONode() {
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
FIBIONode::get_uid_address_parameter_name() const
{
  return "UID_PROM";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
FIBIONode::get_status(bool print_out) const {
	std::stringstream status;
	auto subnodes = read_sub_nodes(getNode("csr.stat"));

	uint32_t sfp_los_flags = read_sfp_los_flags(); // NOLINT(build/unsigned)
	uint32_t sfp_fault_flags = read_sfp_fault_flags(); // NOLINT(build/unsigned)
	
	std::vector<std::pair<std::string, std::string>> sfps_summary;
	sfps_summary.push_back(std::make_pair("SFP LOS flags", format_reg_value(sfp_los_flags, 16)));
	sfps_summary.push_back(std::make_pair("SFP fault flags", format_reg_value(sfp_fault_flags, 16)));

	status << format_reg_table(subnodes, "FIB IO state") << std::endl;
	status << format_reg_table(sfps_summary, "FIB SFPs state");

	if (print_out)
	  TLOG() << status.str();

    return status.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBIONode::reset(const std::string& clock_config_file) const {
	
	// Soft reset
	write_soft_reset_register();
	
	millisleep(1000);

	// Reset I2C
	getNode("csr.ctrl.rstb_i2c").write(0x1);
	getNode("csr.ctrl.rstb_i2c").write(0x0);

	getNode("csr.ctrl.rst").write(0x1);
	getNode("csr.ctrl.rst").write(0x0);

	getClient().dispatch();

	const CarrierType carrier_type = convert_value_to_carrier_type(read_carrier_type());

	if (carrier_type == kCarrierEnclustraA35) {
		// enclustra i2c switch stuff
		try {
			getNode<I2CMasterNode>(m_uid_i2c_bus).get_slave("AX3_Switch").write_i2c(0x01, 0x7f);
		} catch (const std::exception& e) {
      		ers::warning(EnclustraSwitchFailure(ERS_HERE, e));
    	}
	}
	
	// Configure I2C IO expanders
	auto ic_10 = get_i2c_device<I2CExpanderSlave>(m_uid_i2c_bus, "Expander1");
	auto ic_23 = get_i2c_device<I2CExpanderSlave>(m_uid_i2c_bus, "Expander2");

	// Bank 0
	ic_10->set_inversion(0, 0x00);
	
	//  all out, sfp tx disable
	ic_10->set_io(0, 0x00);
	// sfp laser on by default
	ic_10->set_outputs(0, 0x00);

	// Bank 1
	ic_10->set_inversion(1, 0x00);
	// all inputs, sfp fault
	ic_10->set_io(1, 0xff);

	// Bank 0
	ic_23->set_inversion(0, 0x00);
	// pin 0 - out: pll rst, pins 1-4 pll and cdr flags
	ic_23->set_io(0, 0xfe);
	ic_23->set_outputs(0, 0x01);

	// Bank 1
	ic_23->set_inversion(1, 0x00);
	// all inputs, sfp los
	ic_23->set_io(1, 0xff);

	// reset pll via I2C IO expanders
	reset_pll();

	// Upload config file to PLL
	configure_pll(clock_config_file);
	
	//getNode("csr.ctrl.inmux").write(0);
	//getClient().dispatch();
	
// To be removed from firmware address maps also
//	getNode("csr.ctrl.rst_lock_mon").write(0x1);
//	getNode("csr.ctrl.rst_lock_mon").write(0x0);
//	getClient().dispatch();

	TLOG() << "Reset done";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FIBIONode::switch_downstream_mux_channel(uint32_t mux_channel) const { // NOLINT(build/unsigned)
	validate_sfp_id(mux_channel);
	getNode("csr.ctrl.inmux").write(mux_channel);
	getClient().dispatch();	
	TLOG_DEBUG(0) << "SFP input mux set to " << read_active_downstream_mux_channel();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
FIBIONode::read_active_downstream_mux_channel() const {
	auto active_sfp_mux_channel = getNode("csr.ctrl.inmux").read();
	getClient().dispatch();
	return active_sfp_mux_channel.value();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
FIBIONode::get_sfp_status(uint32_t sfp_id, bool print_out) const { // NOLINT(build/unsigned)
	std::stringstream status;
	
	validate_sfp_id(sfp_id);

	std::string sfp_i2c_bus = "i2c_sfp" + std::to_string(sfp_id);
	auto sfp = get_i2c_device<I2CSFPSlave>(sfp_i2c_bus, "SFP_EEProm");
	status << "Fanout SFP " << sfp_id << ":" << std::endl;
	status << sfp->get_status();	
	
	if (print_out)
		TLOG() << status.str();

	return status.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBIONode::switch_sfp_soft_tx_control_bit(uint32_t sfp_id, bool turn_on) const { // NOLINT(build/unsigned)
	validate_sfp_id(sfp_id);

	// on this board the 8 downstream sfps have their own i2c bus
	std::string sfp_i2c_bus = "i2c_sfp" + std::to_string(sfp_id);
	auto sfp = get_i2c_device<I2CSFPSlave>(sfp_i2c_bus, "SFP_EEProm");
	sfp->switch_soft_tx_control_bit(turn_on);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBIONode::reset_pll() const {
	auto ic_23 = get_i2c_device<I2CExpanderSlave>(m_uid_i2c_bus, "Expander2");
	ic_23->set_outputs(0, 0x00);
	ic_23->set_outputs(0, 0x01);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint8_t // NOLINT(build/unsigned)
FIBIONode::read_sfp_los_flags() const {
	auto ic_23 = get_i2c_device<I2CExpanderSlave>(m_uid_i2c_bus, "Expander2");

	uint8_t sfp_los_flags = ic_23->read_inputs(0x01); // NOLINT(build/unsigned)
	return sfp_los_flags;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint8_t // NOLINT(build/unsigned)
FIBIONode::read_sfp_fault_flags() const {
	auto ic_10 = get_i2c_device<I2CExpanderSlave>(m_uid_i2c_bus, "Expander1");
	
	uint8_t sfp_fault_flags = ic_10->read_inputs(0x01); // NOLINT(build/unsigned)
	return sfp_fault_flags;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint8_t 											  // NOLINT(build/unsigned)
FIBIONode::read_sfp_los_flag(uint32_t sfp_id) const { // NOLINT(build/unsigned)
	return read_sfp_los_flags() & (1UL << sfp_id);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint8_t 												// NOLINT(build/unsigned)
FIBIONode::read_sfp_fault_flag(uint32_t sfp_id) const { // NOLINT(build/unsigned)
	return read_sfp_fault_flags() & (1UL << sfp_id);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBIONode::switch_sfp_tx(uint32_t sfp_id, bool turn_on) const { // NOLINT(build/unsigned)
	validate_sfp_id(sfp_id);
	
	auto ic_10 = get_i2c_device<I2CExpanderSlave>(m_uid_i2c_bus, "Expander1");
	uint8_t current_sfp_tx_control_flags = ic_10->read_outputs_config(0); // NOLINT(build/unsigned)

	uint8_t new_sfp_tx_control_flags; // NOLINT(build/unsigned)
	if (turn_on) 
	{
		new_sfp_tx_control_flags = current_sfp_tx_control_flags & ~(1UL << sfp_id);
	}
    else 
    {
    	new_sfp_tx_control_flags = current_sfp_tx_control_flags | (1UL << sfp_id);
    }

    ic_10->set_outputs(0, new_sfp_tx_control_flags);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBIONode::validate_sfp_id(uint32_t sfp_id) const { // NOLINT(build/unsigned)
	// on this board we have 8 downstream SFPs
	if (sfp_id > 7) {
        throw InvalidSFPId(ERS_HERE, format_reg_value(sfp_id));
	}
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//void
//FIBIONode::get_info(timinghardwareinfo::TimingFIBMonitorData& mon_data) const
//{

//   auto stat_subnodes = read_sub_nodes(getNode("csr.stat"));
//   auto ctrl_subnodes = read_sub_nodes(getNode("csr.ctrl"));

//   mon_data.mmcm_ok = stat_subnodes.at("mmcm_ok").value();
//   mon_data.mmcm_sticky = stat_subnodes.at("mmcm_sticky").value();
  
//   mon_data.pll_ok = stat_subnodes.at("pll_ok").value();
//   mon_data.pll_sticky = stat_subnodes.at("pll_sticky").value();

//   mon_data.active_sfp_mux = ctrl_subnodes.at("inmux").value();

//   //mon_data.sfp_los_flags = read_sfp_los_flags();
//   //mon_data.sfp_fault_flags = read_sfp_fault_flags();
// }
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// void
// FIBIONode::get_info(opmonlib::InfoCollector& ci, int level) const
// {

//   if (level >= 2) {
//     timinghardwareinfo::TimingPLLMonitorData pll_mon_data;
//     get_pll()->get_info(pll_mon_data);
//     ci.add(pll_mon_data);
    
//     for (uint i=0; i < 8; ++i)
//     {
//       opmonlib::InfoCollector sfp_ic;
      
// 			std::string sfp_i2c_bus = "i2c_sfp" + std::to_string(i);
// 			auto sfp = get_i2c_device<I2CSFPSlave>(sfp_i2c_bus, "SFP_EEProm");
			      
//       try
//       {
//         sfp->get_info(sfp_ic, level);
//       }
//       catch (timing::SFPUnreachable& e)
//       {
//         // It is valid that an SFP may not be installed, currently no good way of knowing whether they it should be
//         TLOG_DEBUG(2) << "Failed to communicate with SFP " << i <<  " on I2C switch channel " << (1UL << i) << " on i2c bus" << m_sfp_i2c_buses.at(0);
//         continue;
//       }
//       ci.add("sfp_"+std::to_string(i),sfp_ic);
//     }
//   }
//   if (level >= 1) {
//     timinghardwareinfo::TimingFIBMonitorData mon_data;
//     this->get_info(mon_data);
//     ci.add(mon_data);
//   }
// }
//-----------------------------------------------------------------------------
} // namespace timing
} // namespace dunedaq
