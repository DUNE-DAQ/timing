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
  : FanoutIONode(node, "i2c", "i2c", "SI5345", { "PLL", "CDR" }, { "usfp_i2c", "i2c" })
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
PC059IONode::~PC059IONode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
PC059IONode::get_uid_address_parameter_name() const
{
  return "FMC_UID_PROM";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
PC059IONode::get_status(bool print_out) const
{
  std::stringstream status;
  auto subnodes = read_sub_nodes(getNode("csr.stat"));
  status << format_reg_table(subnodes, "PC059 IO state");

  if (print_out)
    TLOG() << std::endl << status.str();
  return status.str();
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
  } catch (const std::exception& e) {
      ers::warning(EnclustraSwitchFailure(ERS_HERE, e));
  }

  // Find the right pll config file
  std::string clock_config_path = get_full_clock_config_file_path(clock_config_file, fanout_mode);
  TLOG() << "PLL configuration file : " << clock_config_path;

  // Upload config file to PLL
  configure_pll(clock_config_path);

  // Reset mmcm
  getNode("csr.ctrl.rst").write(0x1);
  getNode("csr.ctrl.rst").write(0x0);
  getClient().dispatch();

  getNode("csr.ctrl.mux").write(0);
  getClient().dispatch();

  auto sfp_expander = get_i2c_device<I2CExpanderSlave>(m_uid_i2c_bus, "SFPExpander");

  // Set invert registers to default for both banks
  sfp_expander->set_inversion(0, 0x00);
  sfp_expander->set_inversion(1, 0x00);

  // Bank 0 input, bank 1 output
  sfp_expander->set_io(0, 0x00);
  sfp_expander->set_io(1, 0xff);

  // Bank 0 - enable all SFPGs (enable low)
  sfp_expander->set_outputs(0, 0x00);
  TLOG_DEBUG(0) << "SFPs 0-7 enabled";

  // To be removed from firmware address maps also
  //getNode("csr.ctrl.rst_lock_mon").write(0x1);
  //getNode("csr.ctrl.rst_lock_mon").write(0x0);
  //getClient().dispatch();

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
  auto active_sfp_mux_channel = getNode("csr.ctrl.mux").read();
  getClient().dispatch();
  return active_sfp_mux_channel.value();
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

  uint8_t channel_select_byte = 1UL << sfp_id; // NOLINT(build/unsigned)
  getNode<I2CMasterNode>(m_pll_i2c_bus).get_slave("SFP_Switch").write_i2cPrimitive({ channel_select_byte });
  TLOG_DEBUG(0) << "PC059 SFP I2C mux set to " << format_reg_value(sfp_id);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
PC059IONode::get_sfp_status(uint32_t sfp_id, bool print_out) const // NOLINT(build/unsigned)
{
  // on this board the upstream sfp has its own i2c bus, and the 8 downstream sfps are muxed onto the main i2c bus
  std::stringstream status;
  uint32_t sfp_bus_index; // NOLINT(build/unsigned)
  if (sfp_id == 0) {
    sfp_bus_index = 0;
    status << "Upstream SFP:" << std::endl;
  } else if (sfp_id > 0 && sfp_id < 9) {
    switch_sfp_i2c_mux_channel(sfp_id - 1);
    status << "Fanout SFP " << sfp_id - 1 << ":" << std::endl;
    sfp_bus_index = 1;
  } else {
    throw InvalidSFPId(ERS_HERE, format_reg_value(sfp_id));
  }
  auto sfp = get_i2c_device<I2CSFPSlave>(m_sfp_i2c_buses.at(sfp_bus_index), "SFP_EEProm");

  status << sfp->get_status();

  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PC059IONode::switch_sfp_soft_tx_control_bit(uint32_t sfp_id, bool turn_on) const // NOLINT(build/unsigned)
{
  // on this board the upstream sfp has its own i2c bus, and the 8 downstream sfps are muxed onto the main i2c bus
  uint32_t sfp_bus_index; // NOLINT(build/unsigned)
  if (sfp_id == 0) {
    sfp_bus_index = 0;
  } else if (sfp_id > 0 && sfp_id < 9) {
    switch_sfp_i2c_mux_channel(sfp_id - 1);
    sfp_bus_index = 1;
  } else {
    throw InvalidSFPId(ERS_HERE, format_reg_value(sfp_id));
  }
  auto sfp = get_i2c_device<I2CSFPSlave>(m_sfp_i2c_buses.at(sfp_bus_index), "SFP_EEProm");
  sfp->switch_soft_tx_control_bit(turn_on);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PC059IONode::get_info(timinghardwareinfo::TimingPC059MonitorData& mon_data) const
{
  auto subnodes = read_sub_nodes(getNode("csr.stat"));

  mon_data.cdr_lol = subnodes.at("cdr_lol").value();
  mon_data.cdr_los = subnodes.at("cdr_los").value();
  
  mon_data.mmcm_ok = subnodes.at("mmcm_ok").value();
  mon_data.mmcm_sticky = subnodes.at("mmcm_sticky").value();
  
  mon_data.pll_lol = subnodes.at("pll_lol").value();
  mon_data.pll_ok = subnodes.at("pll_ok").value();
  mon_data.pll_sticky = subnodes.at("pll_sticky").value();
  
  mon_data.sfp_los = subnodes.at("sfp_los").value();

  mon_data.ucdr_lol = subnodes.at("ucdr_lol").value();
  mon_data.ucdr_los = subnodes.at("ucdr_los").value();

  mon_data.usfp_flt = subnodes.at("usfp_flt").value();
  mon_data.usfp_los = subnodes.at("usfp_los").value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PC059IONode::get_info(opmonlib::InfoCollector& ci, int level) const
{
  if (level >= 2) {
    timinghardwareinfo::TimingPLLMonitorData pll_mon_data;
    this->get_pll()->get_info(pll_mon_data);
    ci.add(pll_mon_data);

    timinghardwareinfo::TimingSFPMonitorData upstream_sfp_mon_data;
    auto upstream_sfp = get_i2c_device<I2CSFPSlave>(m_sfp_i2c_buses.at(0), "SFP_EEProm");
    upstream_sfp->get_info(upstream_sfp_mon_data);
    opmonlib::InfoCollector upstream_sfp_ic;
    upstream_sfp_ic.add(upstream_sfp_mon_data);
    ci.add("upstream_sfp", upstream_sfp_ic);

    for (uint sfp_id=0; sfp_id < 8; ++sfp_id) {
      TLOG_DEBUG(0) << "checking sfp: " << sfp_id;
      switch_sfp_i2c_mux_channel(sfp_id);
      
      auto sfp = get_i2c_device<I2CSFPSlave>(m_sfp_i2c_buses.at(1), "SFP_EEProm");
      timinghardwareinfo::TimingSFPMonitorData sfp_data;

      try {
        sfp->get_info(sfp_data);
      } catch (timing::SFPUnreachable& e) {
        ers::warning(e);
      }

      opmonlib::InfoCollector sfp_ic;
      sfp_ic.add(sfp_data);
      ci.add("sfp_"+std::to_string(sfp_id), sfp_ic);
    }
  }

  if (level >= 1) {
    timinghardwareinfo::TimingPC059MonitorData mon_data;
    this->get_info(mon_data);
    ci.add(mon_data);
  }  
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq