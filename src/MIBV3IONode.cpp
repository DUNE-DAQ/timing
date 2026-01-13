/**
 * @file MIBV3IONode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/MIBV3IONode.hpp"

#include <string>
#include <math.h>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(MIBV3IONode)

//-----------------------------------------------------------------------------
MIBV3IONode::MIBV3IONode(const uhal::Node& node)
  : IONode(node, "i2c", "i2c", { "PLL" }, { "OSC", "PLL", "EP 0", "EP 1", "EP 2" }, { "sfp0_i2c", "sfp1_i2c", "sfp2_i2c" })
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
MIBV3IONode::~MIBV3IONode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
MIBV3IONode::get_uid_address_parameter_name() const
{
  return "UID_PROM";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
MIBV3IONode::get_status(bool print_out) const
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
MIBV3IONode::reset(const std::string& clock_config_file) const
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
MIBV3IONode::reset(const ClockSource& clock_source) const
{
  IONode::reset(clock_source);

  switch_clock_source(clock_source);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MIBV3IONode::reset_pll() const
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MIBV3IONode::switch_clock_source(const ClockSource& clock_source) const
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
MIBV3IONode::get_sfp_status(uint32_t sfp_id, bool print_out) const { // NOLINT(build/unsigned)
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
bool
MIBV3IONode::clocks_ok() const
{
  std::stringstream status;

  auto states = read_sub_nodes(getNode("csr.stat"));
  bool pll_ok = states.find("pll_ok")->second.value();
  bool mmcm_ok = states.find("mmcm_ok")->second.value();

  TLOG_DEBUG(5) << "pll ok: " << pll_ok << ", mmcm ok: " << mmcm_ok;

  return pll_ok && mmcm_ok;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MIBV3IONode::switch_sfp_soft_tx_control_bit(uint32_t sfp_id, bool turn_on) const { // NOLINT(build/unsigned)
  validate_sfp_id(sfp_id);

  auto sfp = get_i2c_device<I2CSFPSlave>(m_sfp_i2c_buses.at(sfp_id), "SFP_EEProm");
  sfp->switch_soft_tx_control_bit(turn_on);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MIBV3IONode::switch_sfp_tx(uint32_t /*sfp_id*/, bool /*turn_on*/) const // NOLINT(build/unsigned)
{
  // TODO firmware support needed
  //validate_sfp_id(sfp_id);

	//uint8_t current_sfp_tx_control_flags = getNode("csr.ctrl.sfp_tx_disable").read(); // NOLINT(build/unsigned)
  //getClient().dispatch();

	//uint8_t new_sfp_tx_control_flags; // NOLINT(build/unsigned)
	//if (turn_on)
	//{
	//	new_sfp_tx_control_flags = current_sfp_tx_control_flags & ~(1UL << sfp_id);
	//}
  //else
  //{
  //  new_sfp_tx_control_flags = current_sfp_tx_control_flags | (1UL << sfp_id);
  //}

  //getNode("csr.ctrl.sfp_tx_disable").write(new_sfp_tx_control_flags);
  //getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//void
//MIBV3IONode::get_info(timinghardwareinfo::TimingMIBV3MonitorData& mon_data) const
//{
  // TODO
//}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// void
// MIBV3IONode::get_info(opmonlib::InfoCollector& /*ci*/, int /*level*/) const
// {
//   // TO DO
// }
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MIBV3IONode::validate_sfp_id(uint32_t sfp_id) const { // NOLINT(build/unsigned)
  // on this board we have 3 upstream SFPs
  if (sfp_id > 2) {
        throw InvalidSFPId(ERS_HERE, format_reg_value(sfp_id));
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MIBV3IONode::validate_amc_slot(uint32_t amc_slot) const { // NOLINT(build/unsigned)
  if (amc_slot < 1 || amc_slot > 12) {
        throw InvalidAMCSlot(ERS_HERE, format_reg_value(amc_slot, 10));
  }
}
//-----------------------------------------------------------------------------
} // namespace timing
} // namespace dunedaq
