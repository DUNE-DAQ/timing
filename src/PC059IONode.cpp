/**
 * @file PC059IONode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/PC059IONode.hpp"

#include "logging/Logging.hpp"

#include <string>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(PC059IONode)

//-----------------------------------------------------------------------------
PC059IONode::PC059IONode(const uhal::Node& node)
  : FanoutIONode(node, "i2c", "FMC_UID_PROM", "i2c", "SI5345", { "PLL", "CDR" }, { "usfp_i2c", "i2c" })
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
PC059IONode::~PC059IONode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
PC059IONode::get_status(bool print_out) const
{
  std::stringstream lStatus;
  auto subnodes = read_sub_nodes(getNode("csr.stat"));
  lStatus << format_reg_table(subnodes, "PC059 IO state");

  if (print_out)
    TLOG() << std::endl << lStatus.str();
  return lStatus.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PC059IONode::reset(int32_t fanout_mode, const std::string& clock_config_file) const
{

  // Soft reset
  write_soft_reset_register();

  millisleep(1000);

  // Reset PLL and I2C
  getNode("csr.ctrl.pll_rst").write(0x1);
  getNode("csr.ctrl.pll_rst").write(0x0);

  getNode("csr.ctrl.rst_i2c").write(0x1);
  getNode("csr.ctrl.rst_i2c").write(0x0);

  getNode("csr.ctrl.rst_i2cmux").write(0x1);
  getNode("csr.ctrl.rst_i2cmux").write(0x0);

  getClient().dispatch();

  // enclustra i2c switch stuff
  try {
    getNode<I2CMasterNode>(m_uid_i2c_bus).get_slave("AX3_Switch").write_i2c(0x01, 0x7f);
  } catch (...) { // NOLINT
                  // TODO, Eric Flumerfelt <eflumerf@fnal.gov> May-21-2021: Throw appropriate ERS exception
  }

  // Find the right pll config file
  std::string lClockConfigFile = get_full_clock_config_file_path(clock_config_file, fanout_mode);
  TLOG_DEBUG(0) << "PLL configuration file : " << lClockConfigFile;

  // Upload config file to PLL
  configure_pll(lClockConfigFile);

  getNode("csr.ctrl.mux").write(0);
  getClient().dispatch();

  auto lSFPExp = get_i2c_device<I2CExpanderSlave>(m_uid_i2c_bus, "SFPExpander");

  // Set invert registers to default for both banks
  lSFPExp->set_inversion(0, 0x00);
  lSFPExp->set_inversion(1, 0x00);

  // Bank 0 input, bank 1 output
  lSFPExp->set_io(0, 0x00);
  lSFPExp->set_io(1, 0xff);

  // Bank 0 - enable all SFPGs (enable low)
  lSFPExp->set_outputs(0, 0x00);
  TLOG_DEBUG(0) << "SFPs 0-7 enabled";

  getNode("csr.ctrl.rst_lock_mon").write(0x1);
  getNode("csr.ctrl.rst_lock_mon").write(0x0);
  getClient().dispatch();

  TLOG() << "Reset done";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PC059IONode::reset(const std::string& clock_config_file) const
{
  reset(-1, clock_config_file);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PC059IONode::switch_sfp_mux_channel(uint32_t sfp_id) const // NOLINT(build/unsigned)
{
  getNode("csr.ctrl.mux").write(sfp_id);
  getClient().dispatch();

  TLOG_DEBUG(0) << "SFP input mux set to " << format_reg_value(read_active_sfp_mux_channel());
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
PC059IONode::read_active_sfp_mux_channel() const
{
  auto lActiveSFPMUXChannel = getNode("csr.ctrl.mux").read();
  getClient().dispatch();
  return lActiveSFPMUXChannel.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PC059IONode::switch_sfp_i2c_mux_channel(uint32_t sfp_id) const // NOLINT(build/unsigned)
{

  getNode("csr.ctrl.rst_i2cmux").write(0x1);
  getClient().dispatch();
  getNode("csr.ctrl.rst_i2cmux").write(0x0);
  getClient().dispatch();
  millisleep(100);

  uint8_t lChannelSelectByte = 1UL << sfp_id; // NOLINT(build/unsigned)
  getNode<I2CMasterNode>(m_pll_i2c_bus).get_slave("SFP_Switch").write_i2cPrimitive({ lChannelSelectByte });
  TLOG_DEBUG(0) << "PC059 SFP I2C mux set to " << format_reg_value(sfp_id);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
PC059IONode::get_sfp_status(uint32_t sfp_id, bool print_out) const // NOLINT(build/unsigned)
{
  // on this board the upstream sfp has its own i2c bus, and the 8 downstream sfps are muxed onto the main i2c bus
  std::stringstream lStatus;
  uint32_t lSFPBusId; // NOLINT(build/unsigned)
  if (sfp_id == 0) {
    lSFPBusId = 0;
    lStatus << "Upstream SFP:" << std::endl;
  } else if (sfp_id > 0 && sfp_id < 9) {
    switch_sfp_i2c_mux_channel(sfp_id - 1);
    lStatus << "Fanout SFP " << sfp_id - 1 << ":" << std::endl;
    lSFPBusId = 1;
  } else {
    throw InvalidSFPId(ERS_HERE, format_reg_value(sfp_id));
  }
  auto sfp = get_i2c_device<I2CSFPSlave>(m_sfp_i2c_buses.at(lSFPBusId), "SFP_EEProm");

  lStatus << sfp->get_status();

  if (print_out)
    TLOG() << lStatus.str();
  return lStatus.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PC059IONode::switch_sfp_soft_tx_control_bit(uint32_t sfp_id, bool turn_on) const // NOLINT(build/unsigned)
{
  // on this board the upstream sfp has its own i2c bus, and the 8 downstream sfps are muxed onto the main i2c bus
  uint32_t lSFPBusId; // NOLINT(build/unsigned)
  if (sfp_id == 0) {
    lSFPBusId = 0;
  } else if (sfp_id > 0 && sfp_id < 9) {
    switch_sfp_i2c_mux_channel(sfp_id - 1);
    lSFPBusId = 1;
  } else {
    throw InvalidSFPId(ERS_HERE, format_reg_value(sfp_id));
  }
  auto sfp = get_i2c_device<I2CSFPSlave>(m_sfp_i2c_buses.at(lSFPBusId), "SFP_EEProm");
  sfp->switch_soft_tx_control_bit(turn_on);
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq