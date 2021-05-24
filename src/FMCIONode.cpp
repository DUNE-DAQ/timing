/**
 * @file FMCIONode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/FMCIONode.hpp"

#include <string>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(FMCIONode)

//-----------------------------------------------------------------------------
FMCIONode::FMCIONode(const uhal::Node& node)
  : IONode(node, "uid_i2c", "FMC_UID_PROM", "pll_i2c", "i2caddr", { "PLL", "CDR" }, { "sfp_i2c" })
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
FMCIONode::~FMCIONode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
FMCIONode::get_status(bool print_out) const
{
  std::stringstream lStatus;

  auto subnodes = read_sub_nodes(getNode("csr.stat"));
  lStatus << format_reg_table(subnodes, "FMC IO state");

  if (print_out)
    TLOG() << std::endl << lStatus.str();
  return lStatus.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FMCIONode::reset(const std::string& clock_config_file) const
{

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
      getNode<I2CMasterNode>(m_uid_i2c_bus).get_slave("AX3_Switch").write_i2c(0x01, 0x7f);
    } catch (...) { // NOLINT
                    // TODO, Eric Flumerfelt <eflumerf@fnal.gov> May-21-2021: Throw appropriate ERS exception
    }
  }

  // Find the right pll config file
  std::string lClockConfigFile = get_full_clock_config_file_path(clock_config_file);
  TLOG_DEBUG(0) << "PLL configuration file : " << lClockConfigFile;

  // Upload config file to PLL
  configure_pll(lClockConfigFile);

  // Disable sfp tx laser
  getNode("csr.ctrl.sfp_tx_dis").write(0x0);

  getNode("csr.ctrl.rst_lock_mon").write(0x1);
  getNode("csr.ctrl.rst_lock_mon").write(0x0);
  getClient().dispatch();

  TLOG() << "Reset done";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FMCIONode::get_info(timinghardwareinfo::TimingFMCMonitorData& mon_data) const
{

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
FMCIONode::get_info(timinghardwareinfo::TimingFMCMonitorDataDebug& mon_data) const
{

  this->get_pll()->get_info(mon_data.pll_mon_data);
  auto sfp = this->get_i2c_device<I2CSFPSlave>(m_sfp_i2c_buses.at(0), "SFP_EEProm");
  sfp->get_info(mon_data.sfp_mon_data);
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq