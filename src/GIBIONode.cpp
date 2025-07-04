/**
 * @file GIBIONode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/GIBIONode.hpp"

#include <string>
#include <math.h>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(GIBIONode)

//-----------------------------------------------------------------------------
GIBIONode::GIBIONode(const uhal::Node& node)
  : IONode(node, "i2c", "i2c", { "PLL" }, { "PLL", "SFP CDR 0", "SFP CDR 1", "SFP CDR 2", "SFP CDR 3", "SFP CDR 4", "SFP CDR 5", "10 MHz" }, { "i2c", "i2c", "i2c", "i2c", "i2c", "i2c" })
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
GIBIONode::~GIBIONode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
GIBIONode::get_uid_address_parameter_name() const
{
  return "UID_PROM";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
GIBIONode::get_status(bool print_out) const
{
  std::stringstream status;

  auto subnodes = read_sub_nodes(getNode("csr.stat"));
  status << format_reg_table(subnodes, "GIB IO state");

  auto subnodes_2 = read_sub_nodes(getNode("csr.ctrl"));
  status << format_reg_table(subnodes_2, "GIB IO control");
  

  if (print_out)
    TLOG() << std::endl << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::unique_ptr<const SI534xSlave>
GIBIONode::get_pll() const
{
  // enable pll channel 0 only
  set_i2c_mux_channels(0x1);
  return get_i2c_device<SI534xSlave>(m_pll_i2c_bus, m_pll_i2c_device);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
GIBIONode::get_hardware_info(bool print_out) const
{
  // enable pll/uid channel 0 only
  set_i2c_mux_channels(0x1);
  return IONode::get_hardware_info(print_out);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
GIBIONode::set_up_io_infrastructure() const
{
  // enclustra i2c switch stuff
  CarrierType carrier_type = convert_value_to_carrier_type(read_carrier_type());
  if (carrier_type == kCarrierEnclustraA35) {
    try {
      getNode<I2CMasterNode>(m_uid_i2c_bus).get_slave("AX3_Switch").write_i2c(0x01, 0x7f);
    } catch (const std::exception& e) {
      ers::warning(EnclustraSwitchFailure(ERS_HERE, e));
    }
  }

  // Reset I2C switch and expander, active low
  getNode("csr.ctrl.i2c_sw_rst").write(0x0);
  getNode("csr.ctrl.i2c_exten_rst").write(0x0);
  getNode("csr.ctrl.clk_gen_rst").write(0x0);
  getClient().dispatch();

  millisleep(1);

  // End reset 
  getNode("csr.ctrl.i2c_sw_rst").write(0x1);
  getNode("csr.ctrl.i2c_exten_rst").write(0x1);
  getNode("csr.ctrl.clk_gen_rst").write(0x1);
  getClient().dispatch();

  set_i2c_mux_channels(0x1);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
GIBIONode::reset(const std::string& clock_config_file) const
{
  getNode("csr.ctrl.rst").write(0x1);
  getNode("csr.ctrl.rst").write(0x0);
  getClient().dispatch();

  set_up_io_infrastructure();

  getNode("csr.ctrl.gps_clk_en").write(0x0);

  // Set filter to full bandwidth mode A = B = 0x0
  getNode("csr.ctrl.gps_clk_fltr_a").write(0x0);
  getNode("csr.ctrl.gps_clk_fltr_b").write(0x0);
  getClient().dispatch();

  // Upload config file to PLL
  configure_pll(clock_config_file);

  auto sfp_expander_0 = get_i2c_device<I2CExpanderSlave>(m_uid_i2c_bus, "SFPExpander0");
  auto sfp_expander_1 = get_i2c_device<I2CExpanderSlave>(m_uid_i2c_bus, "SFPExpander1");
  
  // Set invert registers to default for both (0,1) banks
  sfp_expander_0->set_inversion(0, 0x00);
  sfp_expander_0->set_inversion(1, 0x00);
  sfp_expander_1->set_inversion(0, 0x00);
  sfp_expander_1->set_inversion(1, 0x00);
  
  // 0: pin set as output, 1: pin set as input
  sfp_expander_0->set_io(0, 0xff); // set all pins of bank 0 as inputs
  sfp_expander_0->set_io(1, 0xff); // set all pins of bank 1 as inputs

  sfp_expander_1->set_io(0, 0xff); // set all pins of bank 0 as inputs
  sfp_expander_1->set_io(1, 0x00); // set all pins of bank 1 as outputs

  // Set SFP disable 
  // Set pins 1-6 low, i.e. enable SFP 1-6 (pins 7,8 unused)
  sfp_expander_1->set_outputs(1, 0xC0);

  TLOG() << "Reset done";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
GIBIONode::reset_pll() const
{
  getNode("csr.ctrl.clk_gen_rst").write(0x0);
  getNode("csr.ctrl.clk_gen_rst").write(0x1);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
GIBIONode::get_sfp_status(uint32_t sfp_id, bool print_out) const { // NOLINT(build/unsigned)
  std::stringstream status;
  
  validate_sfp_id(sfp_id);

  uint8_t i2c_mux_bitmask = 1UL << (sfp_id+1);

  set_i2c_mux_channels(i2c_mux_bitmask);

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
GIBIONode::clocks_ok() const
{
  std::stringstream status;

  auto states = read_sub_nodes(getNode("csr.stat"));
  bool pll_lol = states.find("clk_gen_lol")->second.value();
  bool pll_interrupt = states.find("clk_gen_intr")->second.value();
  bool mmcm_ok = states.find("mmcm_ok")->second.value();
  bool mmcm_10_ok = states.find("mmcm_ok")->second.value();

  TLOG_DEBUG(5) << "pll lol: " << pll_lol << ", pll intr: " << pll_interrupt
                << ", mmcm ok: " << mmcm_ok << ", mmcm 10MHz ok: " << mmcm_10_ok;

  return !pll_lol && mmcm_ok && mmcm_10_ok;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
GIBIONode::switch_sfp_soft_tx_control_bit(uint32_t sfp_id, bool turn_on) const { // NOLINT(build/unsigned)
  validate_sfp_id(sfp_id);

  auto sfp = get_i2c_device<I2CSFPSlave>(m_sfp_i2c_buses.at(sfp_id), "SFP_EEProm");
  sfp->switch_soft_tx_control_bit(turn_on);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
GIBIONode::switch_sfp_tx(uint32_t sfp_id, bool turn_on) const { // NOLINT(build/unsigned)
	validate_sfp_id(sfp_id);

  auto sfp_expander_1 = get_i2c_device<I2CExpanderSlave>(m_uid_i2c_bus, "SFPExpander1");
	uint8_t current_sfp_tx_control_flags = sfp_expander_1->read_outputs_config(1); // NOLINT(build/unsigned)

	uint8_t new_sfp_tx_control_flags; // NOLINT(build/unsigned)
	if (turn_on)
	{
		new_sfp_tx_control_flags = current_sfp_tx_control_flags & ~(1UL << sfp_id);
	}
  else
  {
    new_sfp_tx_control_flags = current_sfp_tx_control_flags | (1UL << sfp_id);
  }

  sfp_expander_1->set_outputs(1, new_sfp_tx_control_flags);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//void
//GIBIONode::get_info(timinghardwareinfo::TimingGIBMonitorData& mon_data) const
//{
  // TODO
//}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// void
// GIBIONode::get_info(opmonlib::InfoCollector& /*ci*/, int /*level*/) const
// {
//   // TO DO
// }
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
GIBIONode::validate_sfp_id(uint32_t sfp_id) const { // NOLINT(build/unsigned)
  // on this board we have 6 SFPs
  if (sfp_id > 5) {
        throw InvalidSFPId(ERS_HERE, format_reg_value(sfp_id));
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
GIBIONode::set_i2c_mux_channels(uint8_t mux_channel_bitmask) const { // NOLINT(build/unsigned)

  uint8_t mux_channel_config = mux_channel_bitmask & 0x7f;
  
  auto i2c_bus = getNode<I2CMasterNode>("i2c");
  i2c_bus.write_i2cPrimitive(0x70, {mux_channel_config});
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq
