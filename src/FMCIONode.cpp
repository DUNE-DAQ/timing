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
  : IONode(node, "uid_i2c", "pll_i2c", "i2caddr", { "PLL", "CDR" }, { "sfp_i2c" })
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
FMCIONode::~FMCIONode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
FMCIONode::get_uid_address_parameter_name() const
{
  CarrierType carrier_type = convert_value_to_carrier_type(read_carrier_type());

  if (carrier_type == kCarrierNexusVideo || carrier_type == kCarrierAFC) {
    return "FMC_UID_PROM_NEXUS";
  } else {
    return "FMC_UID_PROM";
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
FMCIONode::get_status(bool print_out) const
{
  std::stringstream status;

  auto subnodes = read_sub_nodes(getNode("csr.stat"));
  status << format_reg_table(subnodes, "FMC IO state");

  if (print_out)
    TLOG() << std::endl << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FMCIONode::reset(const std::string& clock_config_file) const
{

  write_soft_reset_register();

  millisleep(1000);

  // Reset PLL
  getNode("csr.ctrl.pll_rst").write(0x1);
  getNode("csr.ctrl.pll_rst").write(0x0);
  getClient().dispatch();

  CarrierType carrier_type = convert_value_to_carrier_type(read_carrier_type());

  // enclustra i2c switch stuff
  if (carrier_type == kCarrierEnclustraA35) {
    try {
      getNode<I2CMasterNode>(m_uid_i2c_bus).get_slave("AX3_Switch").write_i2c(0x01, 0x7f);
    } catch (const std::exception& e) {
      ers::warning(EnclustraSwitchFailure(ERS_HERE, e));
    }
  }

  // Find the right pll config file
  std::string clock_config_path = get_full_clock_config_file_path(clock_config_file);
  TLOG() << "PLL configuration file : " << clock_config_path;

  // Upload config file to PLL
  configure_pll(clock_config_path);

  // Reset mmcm
  getNode("csr.ctrl.rst").write(0x1);
  getNode("csr.ctrl.rst").write(0x0);
  getClient().dispatch();

  // Enable sfp tx laser
  getNode("csr.ctrl.sfp_tx_dis").write(0x0);

  // rx edges
  uint32_t cdr_rx_edge = 0x0; // NOLINT(build/unsigned)
  uint32_t sfp_rx_edge = 0x0; // NOLINT(build/unsigned)
  uint32_t rj45_rx_edge = 0x0; // NOLINT(build/unsigned)

  // tx edges
  uint32_t sfp_tx_edge = 0x0; // NOLINT(build/unsigned)
  uint32_t rj45_tx_edge = 0x0; // NOLINT(build/unsigned)

  // rx edges
  getNode("csr.ctrl.cdr_rx_edge").write(cdr_rx_edge);
  getNode("csr.ctrl.sfp_rx_edge").write(sfp_rx_edge);
  getNode("csr.ctrl.rj45_rx_edge").write(rj45_rx_edge);

  // tx edges
  getNode("csr.ctrl.sfp_tx_edge").write(sfp_tx_edge);
  getNode("csr.ctrl.rj45_tx_edge").write(rj45_tx_edge);

  getClient().dispatch();

  TLOG() << "Reset done";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FMCIONode::reset(int32_t /*fanout_mode*/, // NOLINT(build/unsigned)
                     const std::string& clock_config_file) const
{
  reset(clock_config_file);
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
FMCIONode::get_info(opmonlib::InfoCollector& ci, int level) const
{
  if (level >= 2) {
    timinghardwareinfo::TimingPLLMonitorData pll_mon_data;
    this->get_pll()->get_info(pll_mon_data);
    ci.add(pll_mon_data);

    timinghardwareinfo::TimingSFPMonitorData sfp_mon_data;
    auto sfp = this->get_i2c_device<I2CSFPSlave>(m_sfp_i2c_buses.at(0), "SFP_EEProm");
    sfp->get_info(sfp_mon_data);
    ci.add(sfp_mon_data);
  }
  if (level >= 1) {
    timinghardwareinfo::TimingFMCMonitorData mon_data;
    this->get_info(mon_data);
    ci.add(mon_data);
  }
}
//-----------------------------------------------------------------------------
} // namespace timing
} // namespace dunedaq
