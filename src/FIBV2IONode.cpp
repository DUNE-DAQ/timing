/**
 * @file FIBV2IONode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/FIBV2IONode.hpp"

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(FIBV2IONode)

//-----------------------------------------------------------------------------
FIBV2IONode::FIBV2IONode(const uhal::Node& aNode) :
	IONode(aNode, "i2c", "i2c", "SI5345", {"PLL", "BKP DATA"}, {"i2c_sfp0", "i2c_sfp1", "i2c_sfp2", "i2c_sfp3", "i2c_sfp4", "i2c_sfp5", "i2c_sfp6", "i2c_sfp7"}) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
FIBV2IONode::~FIBV2IONode() {
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
FIBV2IONode::get_uid_address_parameter_name() const
{
  return "UID_PROM";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
FIBV2IONode::get_status(bool print_out) const
{
	std::stringstream status;
	auto subnodes = read_sub_nodes(getNode("csr.stat"));
	status << format_reg_table(subnodes, "FIB IO state") << std::endl;

	if (print_out)
	  TLOG() << status.str();

    return status.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBV2IONode::reset(const ClockSource& clock_source) const {
	
	// Soft reset
	write_soft_reset_register();
	
	millisleep(1000);

	getNode("csr.ctrl.rst").write(0x1);
	getNode("csr.ctrl.rst").write(0x0);

	getClient().dispatch();

	getNode("csr.ctrl.clk_enable").write(0x0);

	if (clock_source == kInput0)
	{
		getNode("csr.ctrl.clk_select").write(0x0);
	}
	else if (clock_source == kInput1)
	{
		getNode("csr.ctrl.clk_select").write(0x1);
	}
	else
	{
		// TODO :throw something here
	}

	TLOG() << "Reset done";
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
FIBV2IONode::get_sfp_status(uint32_t sfp_id, bool print_out) const { // NOLINT(build/unsigned)
	std::stringstream status;
	
	validate_sfp_id(sfp_id);

	std::string sfp_i2c_bus = "i2c_sfp" + std::to_string(sfp_id);
	auto sfp = get_i2c_device<I2CSFPSlave>(sfp_i2c_bus, "SFP_EEProm");
	status << "FIB V2 SFP " << sfp_id << ":" << std::endl;
	status << sfp->get_status();	
	
	if (print_out)
		TLOG() << status.str();

	return status.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBV2IONode::switch_sfp_soft_tx_control_bit(uint32_t sfp_id, bool turn_on) const { // NOLINT(build/unsigned)
	validate_sfp_id(sfp_id);

	// on this board the 8 downstream sfps have their own i2c bus
	std::string sfp_i2c_bus = "i2c_sfp" + std::to_string(sfp_id);
	auto sfp = get_i2c_device<I2CSFPSlave>(sfp_i2c_bus, "SFP_EEProm");
	sfp->switch_soft_tx_control_bit(turn_on);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBV2IONode::switch_sfp_tx(uint32_t sfp_id, bool turn_on) const { // NOLINT(build/unsigned)
	validate_sfp_id(sfp_id);
	
	uint8_t current_sfp_tx_control_flags = getNode("csr.ctrl.sfp_tx_disable").read(); // NOLINT(build/unsigned)
    getClient().dispatch();

	uint8_t new_sfp_tx_control_flags; // NOLINT(build/unsigned)
	if (turn_on) 
	{
		new_sfp_tx_control_flags = current_sfp_tx_control_flags & ~(1UL << sfp_id);
	}
    else 
    {
    	new_sfp_tx_control_flags = current_sfp_tx_control_flags | (1UL << sfp_id);
    }

    getNode("csr.ctrl.sfp_tx_disable").write(new_sfp_tx_control_flags);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FIBV2IONode::validate_sfp_id(uint32_t sfp_id) const { // NOLINT(build/unsigned)
	// on this board we have 8 downstream SFPs
	if (sfp_id > 7) {
        throw InvalidSFPId(ERS_HERE, format_reg_value(sfp_id));
	}
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//void
//FIBV2IONode::get_info(timinghardwareinfo::TimingFIBMonitorData& mon_data) const
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
// FIBV2IONode::get_info(opmonlib::InfoCollector& ci, int level) const
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
