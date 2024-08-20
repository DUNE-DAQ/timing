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
  : IONode(node, "i2c", "i2c", { "PLL" }, { "OSC", "PLL", "EP 0", "EP 1", "EP 2" }, { "sfp0_i2c", "sfp1_i2c", "sfp2_i2c" })
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
MIBV2IONode::reset(const std::string& clock_config_file) const
{
  write_soft_reset_register();

  millisleep(1000);

  // Upload config file to PLL
  configure_pll(clock_config_file);

  // Reset mmcm
  getNode("csr.ctrl.rst").write(0x1);
  getNode("csr.ctrl.rst").write(0x0);

  getClient().dispatch();

  TLOG() << "Reset done";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MIBV2IONode::reset(const ClockSource& clock_source) const
{
  IONode::reset(clock_source);

  switch_clock_source(clock_source);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MIBV2IONode::switch_clock_source(const ClockSource& clock_source) const
{
  if (clock_source != kFreeRun)
  {
    getNode("csr.ctrl.pll_in_sel").write(clock_source);
    getClient().dispatch();
  }
}
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
//void
//MIBV2IONode::get_info(timinghardwareinfo::TimingMIBV2MonitorData& mon_data) const
//{
  // TODO
//}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// void
// MIBV2IONode::get_info(opmonlib::InfoCollector& /*ci*/, int /*level*/) const
// {
//   // TO DO
// }
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
