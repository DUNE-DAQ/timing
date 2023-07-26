/**
 * @file MIBV2IONode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/MIBV2IONode.hpp"

#include <string>
#include <math.h>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(MIBV2IONode)

//-----------------------------------------------------------------------------
MIBV2IONode::MIBV2IONode(const uhal::Node& node)
  : FanoutIONode(node, "i2c", "i2c", { "PLL" }, { "OSC", "PLL", "EP 0", "EP 1", "EP 2" }, { "sfp0_i2c", "sfp1_i2c", "sfp2_i2c" })
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
MIBV2IONode::~MIBV2IONode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
MIBV2IONode::get_uid_address_parameter_name() const
{
  return "UID_PROM";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
MIBV2IONode::get_status(bool print_out) const
{
  std::stringstream status;

  auto subnodes = read_sub_nodes(getNode("csr.stat"));
  status << format_reg_table(subnodes, "MIB IO state");

  if (print_out)
    TLOG() << std::endl << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MIBV2IONode::reset(int32_t fanout_mode, const std::string& clock_config_file) const
{
  
  write_soft_reset_register();

  millisleep(1000);
  
  // Find the right pll config file
  std::string clock_config_path = get_full_clock_config_file_path(clock_config_file, fanout_mode);
  TLOG() << "PLL configuration file : " << clock_config_path;

  // Upload config file to PLL
  configure_pll(clock_config_path);

  // Reset mmcm
  getNode("csr.ctrl.rst").write(0x1);
  getNode("csr.ctrl.rst").write(0x0);

  getClient().dispatch();

  TLOG() << "Reset done";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MIBV2IONode::reset(const std::string& clock_config_file) const
{
  reset(-1, clock_config_file);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MIBV2IONode::switch_downstream_mux_channel(uint32_t mux_channel) const // NOLINT(build/unsigned)
{
//  validate_amc_slot(mux_channel);
//  uint16_t amc_in_bit = 0x1 << (mux_channel-1);
//  getNode("io_select.csr.ctrl.amc_in").write(amc_in_bit);
//  
//  TLOG_DEBUG(3) << " MIB downstream AMC (in) " << mux_channel << " enabled";
//  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
MIBV2IONode::read_active_downstream_mux_channel() const
{
//  auto active_mux_channel_bits = getNode("io_select.csr.ctrl.amc_in").read();
//  getClient().dispatch();
//  double mux = log2(active_mux_channel_bits.value());
//  return mux+1;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//void
//MIBV2IONode::switch_upstream_mux_channel(uint32_t mux_channel) const // NOLINT(build/unsigned)
//{
//  getNode("io_select.csr.ctrl.usfp_src").write(mux_channel);
//  TLOG_DEBUG(3) << " MIB upstream SFP (in) " << mux_channel << " enabled";
//  // TODO what about clock config?
//  getClient().dispatch();
//}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//uint32_t // NOLINT(build/unsigned)
//MIBV2IONode::read_active_upstream_mux_channel() const
//{
//  auto active_sfp_mux_channel = getNode("io_select.csr.ctrl.usfp_src").read();
//  getClient().dispatch();
//  return active_sfp_mux_channel.value();
//}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
MIBV2IONode::get_sfp_status(uint32_t sfp_id, bool print_out) const { // NOLINT(build/unsigned)
  std::stringstream status;
  
  validate_sfp_id(sfp_id);

  auto sfp = get_i2c_device<I2CSFPSlave>(m_sfp_i2c_buses.at(sfp_id), "SFP_EEProm");

  status << "SFP " << sfp_id << ":" << std::endl;
  status << sfp->get_status();

  if (print_out)
    TLOG() << status.str();

  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MIBV2IONode::switch_sfp_soft_tx_control_bit(uint32_t sfp_id, bool turn_on) const { // NOLINT(build/unsigned)
  validate_sfp_id(sfp_id);

  auto sfp = get_i2c_device<I2CSFPSlave>(m_sfp_i2c_buses.at(sfp_id), "SFP_EEProm");
  sfp->switch_soft_tx_control_bit(turn_on);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MIBV2IONode::get_info(timinghardwareinfo::TimingMIBMonitorData& mon_data) const
{

  auto subnodes = read_sub_nodes(getNode("csr.stat"));

  mon_data.cdr_0_lol = subnodes.at("cdr_lol").value() & 0x1;
  mon_data.cdr_1_lol = subnodes.at("cdr_lol").value() & 0x2;

  mon_data.cdr_0_los = subnodes.at("cdr_los").value() & 0x1;
  mon_data.cdr_1_los = subnodes.at("cdr_los").value() & 0x2;

  mon_data.mmcm_ok = subnodes.at("mmcm_ok").value();
  mon_data.mmcm_sticky = subnodes.at("mmcm_sticky").value();
  
  mon_data.sfp_0_flt = subnodes.at("sfp_flt").value() & 0x1;
  mon_data.sfp_1_flt = subnodes.at("sfp_flt").value() & 0x2;

  mon_data.sfp_0_los = subnodes.at("sfp_los").value() & 0x1;
  mon_data.sfp_1_los = subnodes.at("sfp_los").value() & 0x2;

  // TODO 3rd SFP?
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MIBV2IONode::get_info(opmonlib::InfoCollector& ci, int level) const
{
//  auto i2c_switch = get_i2c_device<I2C9546SwitchSlave>("i2c", "TCA9546_Switch");
//
//  if (level >= 2) {
//    i2c_switch->set_channels_states(8);
//
//    timinghardwareinfo::TimingPLLMonitorData pll_mon_data;
//    get_pll()->get_info(pll_mon_data);
//    ci.add(pll_mon_data);
//    
//    for (uint i=0; i < 3; ++i)
//    {
//      opmonlib::InfoCollector sfp_ic;
//      
//      // enable i2c path for sfp
//      i2c_switch->set_channels_states(1UL << i);
//
//      auto sfp = this->get_i2c_device<I2CSFPSlave>(m_sfp_i2c_buses.at(0), "SFP_EEProm");
//      
//      try
//      {
//        sfp->get_info(sfp_ic, level);
//      }
//      catch (timing::SFPUnreachable& e)
//      {
//        // It is valid that an SFP may not be installed, currently no good way of knowing whether they it should be
//        TLOG_DEBUG(2) << "Failed to communicate with SFP " << i <<  " on I2C switch channel " << (1UL << i) << " on i2c bus" << m_sfp_i2c_buses.at(0);
//        continue;
//      }
//      ci.add("sfp_"+std::to_string(i),sfp_ic);
//    }
//    i2c_switch->set_channels_states(8);
//  }
//  if (level >= 1) {
//    timinghardwareinfo::TimingMIBMonitorData mon_data;
//    this->get_info(mon_data);
//    ci.add(mon_data);
//  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MIBV2IONode::validate_sfp_id(uint32_t sfp_id) const { // NOLINT(build/unsigned)
  // on this board we have 3 upstream SFPs
  if (sfp_id > 2) {
        throw InvalidSFPId(ERS_HERE, format_reg_value(sfp_id));
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MIBV2IONode::validate_amc_slot(uint32_t amc_slot) const { // NOLINT(build/unsigned)
  if (amc_slot < 1 || amc_slot > 12) {
        throw InvalidAMCSlot(ERS_HERE, format_reg_value(amc_slot, 10));
  }
}
//-----------------------------------------------------------------------------
} // namespace timing
} // namespace dunedaq
