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
  : IONode(node, "i2c", "i2c", { "PLL" }, { "PLL", "CDR 0", "CDR 1" }, { "sfp_i2c" })
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
MIBIONode::reset(int32_t fanout_mode, const std::string& clock_config_file) const
{
  
  write_soft_reset_register();

  millisleep(1000);

  auto i2c_switch = get_i2c_device<I2C9546SwitchSlave>("i2c", "TCA9546_Switch");

  // enable pll channel (#3) only
  i2c_switch->set_channels_states(8);
  
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
MIBIONode::reset(const std::string& clock_config_file) const
{
  reset(-1, clock_config_file);
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
  if (level >= 2) {
    timinghardwareinfo::TimingPLLMonitorData pll_mon_data;
    this->get_pll()->get_info(pll_mon_data);
    ci.add(pll_mon_data);

    auto i2c_switch = get_i2c_device<I2C9546SwitchSlave>("i2c", "TCA9546_Switch");
    
    for (uint i=0; i < 3; ++i)
    {
      opmonlib::InfoCollector sfp_ic;
      
      // enable i2c path for sfp
      i2c_switch->enable_channel(i);

      auto sfp = this->get_i2c_device<I2CSFPSlave>(m_sfp_i2c_buses.at(0), "SFP_EEProm");
      sfp->get_info(sfp_ic, level);

      // disable i2c path for sfp
      i2c_switch->disable_channel(i);
      
      ci.add("sfp_"+std::to_string(i),sfp_ic);
    }
  }
  if (level >= 1) {
    timinghardwareinfo::TimingMIBMonitorData mon_data;
    this->get_info(mon_data);
    ci.add(mon_data);
  }
}
//-----------------------------------------------------------------------------
} // namespace timing
} // namespace dunedaq
