/**
 * @file GIBV2IONode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/GIBV2IONode.hpp"

#include <string>
#include <math.h>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(GIBV2IONode)

//-----------------------------------------------------------------------------
GIBV2IONode::GIBV2IONode(const uhal::Node& node)
  : GIBIONode(node, "i2c", "i2c", { "PLL" }, { "PLL", "SFP CDR 0", "SFP CDR 1", "SFP CDR 2", "SFP CDR 3", "SFP CDR 4", "SFP CDR 5", "10 MHz" }, { "i2c", "i2c", "i2c", "i2c", "i2c", "i2c", "i2c" })
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
GIBV2IONode::~GIBV2IONode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint8_t
GIBV2IONode::read_sfps_los() const { // NOLINT(build/unsigned)
  uint32_t expander_bits = read_io_expanders();

  // A-CLK LOS is 1st bit
  uint8_t los_bits = static_cast<uint8_t>(expander_bits & 0b01);

  for (uint8_t sfp = 0; sfp<6; sfp++) {
    // Each SFP has 4 bits, the 3rd bit is the LOS
    los_bits = (los_bits << 1) + (expander_bits & (1 << (2 + 20 - 4*sfp)));
  }

  return los_bits;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint8_t
GIBV2IONode::read_sfps_fault() const { // NOLINT(build/unsigned)
  uint32_t expander_bits = read_io_expanders();

  // A-CLK fault is 2nd bit
  uint8_t fault_bits = static_cast<uint8_t>(expander_bits & 0b10);

  for (uint8_t sfp = 0; sfp<6; sfp++) {
    // Each SFP has 4 bits, the 4th bit is the fault
    fault_bits = (fault_bits << 1) + (expander_bits & (1 << (3 + 20 - 4*sfp)));
  }

  return fault_bits;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
GIBV2IONode::clocks_ok() const
{
  std::stringstream status;

  auto states = read_sub_nodes(getNode("csr.stat"));
  bool pll_lol = states.find("clk_gen_lol")->second.value();
  //bool pll_interrupt = states.find("clk_gen_intr")->second.value();
  bool mmcm_ok = states.find("mmcm_ok")->second.value();

  TLOG_DEBUG(5) << "pll lol: " << pll_lol << ", mmcm ok: " << mmcm_ok;

  return !pll_lol && mmcm_ok;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
GIBV2IONode::switch_sfp_tx(uint32_t sfp_id, bool turn_on) const { // NOLINT(build/unsigned)
	validate_sfp_id(sfp_id);

  // A-CLK is the 7th SFP, but is in slot 7 not 6
  // TODO make this a map dlindebaum 25/10/02
  sfp_id = sfp_id + (sfp_id/6);

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

} // namespace timing
} // namespace dunedaq
