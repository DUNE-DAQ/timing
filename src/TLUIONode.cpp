/**
 * @file TLUIONode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/TLUIONode.hpp"

#include <string>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(TLUIONode)

//-----------------------------------------------------------------------------
TLUIONode::TLUIONode(const uhal::Node& node)
  : IONode(node, "i2c", "UID_PROM", "i2c", "SI5345", { "PLL" }, {})
  , m_dac_devices({ "DAC1", "DAC2" })
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
TLUIONode::~TLUIONode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
TLUIONode::get_status(bool print_out) const
{
  auto subnodes = read_sub_nodes(getNode("csr.stat"));
  std::stringstream lStatus;
  lStatus << format_reg_table(subnodes, "TLU IO state");

  if (print_out)
    TLOG() << lStatus.str();
  return lStatus.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
TLUIONode::reset(const std::string& clock_config_file) const
{
  // Soft reset
  write_soft_reset_register();

  millisleep(1000);

  // Reset PLL and I2C
  getNode("csr.ctrl.pll_rst").write(0x1);
  getNode("csr.ctrl.pll_rst").write(0x0);

  getNode("csr.ctrl.rst_i2c").write(0x1);
  getNode("csr.ctrl.rst_i2c").write(0x0);

  getClient().dispatch();

  // enclustra i2c switch stuff
  try {
    getNode<I2CMasterNode>(m_uid_i2c_bus).get_slave("AX3_Switch").write_i2c(0x01, 0x7f);
  } catch (...) { // NOLINT
                  // TODO, Eric Flumerfelt <eflumerf@fnal.gov> May-21-2021: Throw appropriate ERS exception
  }

  // Find the right pll config file
  std::string lClockConfigFile = get_full_clock_config_file_path(clock_config_file);
  TLOG_DEBUG(0) << "PLL configuration file : " << lClockConfigFile;

  // Upload config file to PLL
  configure_pll(lClockConfigFile);

  // Tweak the PLL swing
  auto lSIChip = get_pll();
  lSIChip->write_i2cArray(0x113, { 0x9, 0x33 });

  // configure tlu io expanders
  auto lIC6 = get_i2c_device<I2CExpanderSlave>(m_uid_i2c_bus, "Expander1");
  auto lIC7 = get_i2c_device<I2CExpanderSlave>(m_uid_i2c_bus, "Expander2");

  // Bank 0
  lIC6->set_inversion(0, 0x00);
  lIC6->set_io(0, 0x00);
  lIC6->set_outputs(0, 0x00);

  // Bank 1
  lIC6->set_inversion(1, 0x00);
  lIC6->set_io(1, 0x00);
  lIC6->set_outputs(1, 0x88);

  // Bank 0
  lIC7->set_inversion(0, 0x00);
  lIC7->set_io(0, 0x00);
  lIC7->set_outputs(0, 0xf0);

  // Bank 1
  lIC7->set_inversion(1, 0x00);
  lIC7->set_io(1, 0x00);
  lIC7->set_outputs(1, 0xf0);

  // BI signals are NIM
  uint32_t lBISignalThreshold = 0x589D; // NOLINT(build/unsigned)

  configure_dac(0, lBISignalThreshold);
  configure_dac(1, lBISignalThreshold);

  TLOG_DEBUG(0) << "DAC1 and DAC2 set to " << std::hex << lBISignalThreshold;

  getNode("csr.ctrl.rst_lock_mon").write(0x1);
  getNode("csr.ctrl.rst_lock_mon").write(0x0);
  getClient().dispatch();

  TLOG() << "Reset done";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
TLUIONode::configure_dac(uint32_t dac_id, uint32_t dac_value, bool internal_ref) const // NOLINT(build/unsigned)
{
  std::string lDACDevice;
  try {
    lDACDevice = m_dac_devices.at(dac_id);
  } catch (const std::out_of_range& e) {
    throw InvalidDACId(ERS_HERE, format_reg_value(dac_id));
  }
  auto lDAC = get_i2c_device<DACSlave>(m_uid_i2c_bus, lDACDevice);
  lDAC->set_interal_ref(internal_ref);
  lDAC->set_dac(7, dac_value);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
TLUIONode::get_sfp_status(uint32_t /*sfp_id*/, bool /*print_out*/) const // NOLINT(build/unsigned)
{
  TLOG() << "TLU does not support SFP I2C";
  return "";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
TLUIONode::switch_sfp_soft_tx_control_bit(uint32_t /*sfp_id*/, bool /*turn_on*/) const // NOLINT(build/unsigned)
{
  TLOG() << "TLU does not support SFP I2C";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
TLUIONode::get_info(timinghardwareinfo::TimingTLUMonitorData& mon_data) const
{

  auto subnodes = read_sub_nodes(getNode("csr.stat"));

  mon_data.cdr_lol = subnodes.at("cdr_lol");
  mon_data.cdr_los = subnodes.at("cdr_los");
  mon_data.mmcm_ok = subnodes.at("mmcm_ok");
  mon_data.mmcm_sticky = subnodes.at("mmcm_sticky");
  mon_data.pll_ok = subnodes.at("pll_ok");
  mon_data.pll_sticky = subnodes.at("pll_sticky");
  mon_data.sfp_flt = subnodes.at("sfp_fault");
  mon_data.sfp_los = subnodes.at("sfp_los");
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
TLUIONode::get_info(timinghardwareinfo::TimingTLUMonitorDataDebug& mon_data) const
{

  this->get_pll()->get_info(mon_data.pll_mon_data);
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq