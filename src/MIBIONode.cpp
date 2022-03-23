/**
 * @file MIBIONode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/MIBIONode.hpp"

#include <string>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(MIBIONode)

//-----------------------------------------------------------------------------
MIBIONode::MIBIONode(const uhal::Node& node)
  : IONode(node, "i2c", "i2c", { "PLL" }, { "PLL", "CDR 0", "CDR 1" }, { "i2c" })
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
MIBIONode::~MIBIONode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
MIBIONode::get_uid_address_parameter_name() const
{
  return "UID_PROM";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
MIBIONode::get_status(bool print_out) const
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
MIBIONode::configure_pll(const std::string& clock_config_file) const
{
  // enable pll channel (#3) only
  auto i2c_switch = get_i2c_device<I2C9546SwitchSlave>("i2c", "TCA9546_Switch");
  i2c_switch->set_channels_states(8);
  IONode::configure_pll(clock_config_file);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
MIBIONode::get_pll_status(bool print_out) const
{
  // enable pll channel (#3) only
  auto i2c_switch = get_i2c_device<I2C9546SwitchSlave>("i2c", "TCA9546_Switch");
  i2c_switch->set_channels_states(8);
  return IONode::get_pll_status(print_out);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MIBIONode::reset(int32_t fanout_mode, const std::string& clock_config_file) const
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

  getNode("io_select.csr.ctrl.amc_out").write(0xfff);
  getNode("io_select.csr.ctrl.amc_in").write(0x0);
  getNode("io_select.csr.ctrl.usfp_src").write(0x0);

  getClient().dispatch();

  TLOG() << "Reset done";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MIBIONode::reset(const std::string& clock_config_file) const
{
  reset(-1, clock_config_file);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
MIBIONode::get_sfp_status(uint32_t sfp_id, bool print_out) const { // NOLINT(build/unsigned)
  std::stringstream status;
  
  validate_sfp_id(sfp_id);

  // enable i2c path for sfp
  auto i2c_switch = get_i2c_device<I2C9546SwitchSlave>("i2c", "TCA9546_Switch");
  i2c_switch->set_channels_states(1UL << sfp_id);

  auto sfp = get_i2c_device<I2CSFPSlave>(m_sfp_i2c_buses.at(0), "SFP_EEProm");

  status << "SFP " << sfp_id << ":" << std::endl;
  
  try
  {
    status << sfp->get_status();
  }
  catch(...)
  {
    i2c_switch->set_channels_states(8);
    throw;
  }

  i2c_switch->set_channels_states(8);

  if (print_out)
    TLOG() << status.str();

  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MIBIONode::switch_sfp_soft_tx_control_bit(uint32_t sfp_id, bool turn_on) const { // NOLINT(build/unsigned)
  validate_sfp_id(sfp_id);

  auto i2c_switch = get_i2c_device<I2C9546SwitchSlave>("i2c", "TCA9546_Switch");
  i2c_switch->set_channels_states(1UL << sfp_id);
  auto sfp = get_i2c_device<I2CSFPSlave>(m_sfp_i2c_buses.at(0), "SFP_EEProm");
  sfp->switch_soft_tx_control_bit(turn_on);
  i2c_switch->set_channels_states(8);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MIBIONode::get_info(timinghardwareinfo::TimingMIBMonitorData& mon_data) const
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
MIBIONode::get_info(opmonlib::InfoCollector& ci, int level) const
{
    auto i2c_switch = get_i2c_device<I2C9546SwitchSlave>("i2c", "TCA9546_Switch");

  if (level >= 2) {
    i2c_switch->set_channels_states(8);

    timinghardwareinfo::TimingPLLMonitorData pll_mon_data;
    get_pll()->get_info(pll_mon_data);
    ci.add(pll_mon_data);
    
    for (uint i=0; i < 3; ++i)
    {
      opmonlib::InfoCollector sfp_ic;
      
      // enable i2c path for sfp
      i2c_switch->set_channels_states(1UL << i);

      auto sfp = this->get_i2c_device<I2CSFPSlave>(m_sfp_i2c_buses.at(0), "SFP_EEProm");
      
      try
      {
        sfp->get_info(sfp_ic, level);
      }
      catch (timing::SFPUnreachable& e)
      {
        // It is valid that an SFP may not be installed, currently no good way of knowing whether they it should be
        TLOG_DEBUG(2) << "Failed to communicate with SFP " << i <<  " on I2C switch channel " << (1UL << i) << " on i2c bus" << m_sfp_i2c_buses.at(0);
        continue;
      }
      ci.add("sfp_"+std::to_string(i),sfp_ic);
    }
    i2c_switch->set_channels_states(8);
  }
  if (level >= 1) {
    timinghardwareinfo::TimingMIBMonitorData mon_data;
    this->get_info(mon_data);
    ci.add(mon_data);
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MIBIONode::validate_sfp_id(uint32_t sfp_id) const { // NOLINT(build/unsigned)
  // on this board we have 3 downstream SFPs
  if (sfp_id > 2) {
        throw InvalidSFPId(ERS_HERE, format_reg_value(sfp_id));
  }
}
//-----------------------------------------------------------------------------
} // namespace timing
} // namespace dunedaq
