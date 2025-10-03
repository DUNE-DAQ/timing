/**
 * @file GIBV3IONode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/GIBV3IONode.hpp"

#include <string>
#include <math.h>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(GIBV3IONode)

//-----------------------------------------------------------------------------
GIBV3IONode::GIBV3IONode(const uhal::Node& node)
  : GIBV2IONode(node)
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
GIBV3IONode::~GIBV3IONode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
GIBV3IONode::get_status(bool print_out) const
{
  std::stringstream status;

  auto subnodes = read_sub_nodes(getNode("csr.stat"));
  status << format_reg_table(subnodes, "GIB IO state");

  auto subnodes_2 = read_sub_nodes(getNode("csr.ctrl"));
  status << format_reg_table(subnodes_2, "GIB IO control");

  uint8_t sfp_los = read_sfps_los();
  uint8_t sfp_fault = read_sfps_fault();

  std::vector<std::string> sfp_vec;
  std::vector<uint8_t> los_vec;
  std::vector<uint8_t> fault_vec;

  for (int i=0; i<n_sfps; i++) {
    sfp_vec.push_back(to_string(i));
    // L is 0x4C, H is L - 4
    los_vec.push_back(0x4C - 4*((sfp_los >> i) & 1));
    fault_vec.push_back(0x4C - 4*((sfp_fault >> i) & 1));
  }
  
  status << "-----IO expander------" << std::endl;
  status << "SFP:   " << vec_fmt(sfp_vec) << std::endl;
  status << "LOS:   " << vec_fmt(los_vec) << std::endl;
  status << "Fault: " << vec_fmt(fault_vec) << std::endl;

  // removed temperature readout

  if (print_out)
    TLOG() << std::endl << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t
GIBV3IONode::read_io_expanders() const { // NOLINT(build/unsigned)
  auto sfp_expander_0 = get_i2c_device<I2CExpanderSlave>(m_uid_i2c_bus, "SFPExpander0");

  uint32_t expander_bits = sfp_expander_0->read_inputs(1);
  expander_bits = (expander_bits << 8) + sfp_expander_0->read_inputs(0);

  return expander_bits;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint8_t
GIBV3IONode::read_sfps_los() const { // NOLINT(build/unsigned)
  uint32_t expander_bits = read_io_expanders();

  // A-CLK LOS is 1st bit
  uint8_t los_bits = static_cast<uint8_t>(expander_bits & 0b01);

  uint32_t los_bitmask = 0x003f;

  uhal::ValWord<uint32_t> los_reg_data = getNode("csr.stat.sfp_los").read();
  getClient().dispatch();

  los_bits = (los_bits << 6) + static_cast<uint8_t>(los_reg_data & los_bitmask);

  return los_bits;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint8_t
GIBV3IONode::read_sfps_fault() const { // NOLINT(build/unsigned)
  uint32_t expander_bits = read_io_expanders();

  // A-CLK fault is 2nd bit
  uint8_t fault_bits = static_cast<uint8_t>(expander_bits & 0b10);

  for (uint8_t sfp = 0; sfp<6; sfp++) {
    // SFP faults are 0-5 on second bus of first expander
    // Adds the SFPs in reverse order
    fault_bits = (fault_bits << 1) + ((expander_bits >> (8 + 5 - sfp)) & 1);
  }

  return fault_bits;
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq
