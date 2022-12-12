/**
 * @file DACNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/DACNode.hpp"

namespace dunedaq {
namespace timing {

// uHAL Node registation
UHAL_REGISTER_DERIVED_NODE(DACNode)

//-----------------------------------------------------------------------------
DACSlave::DACSlave(const I2CMasterNode* i2c_master, uint8_t address) // NOLINT(build/unsigned)
  : I2CSlave(i2c_master, address)
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
DACSlave::set_interal_ref(bool internal_ref) const
{
  this->write_i2cArray(0x38, { 0x0, internal_ref });
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
DACSlave::set_dac(uint8_t channel, uint32_t code) const // NOLINT(build/unsigned)
{

  if (channel > 7) {
    throw DACChannelOutOfRange(ERS_HERE, std::to_string(channel));
  }

  if (code > 0xffff) {
    throw DACValueOutOfRange(ERS_HERE, std::to_string(code));
  }

  uint32_t address = 0x18 + (channel & 0x7); // NOLINT(build/unsigned)

  this->write_i2cArray(address, { (uint8_t)((code >> 8) & 0xff), (uint8_t)(code & 0xff) }); // NOLINT(build/unsigned)
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
DACNode::DACNode(const uhal::Node& node)
  : I2CMasterNode(node)
  , DACSlave(this, this->get_slave_address("i2caddr"))
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
DACNode::DACNode(const DACNode& node)
  : I2CMasterNode(node)
  , DACSlave(this, this->get_slave_address("i2caddr"))
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
DACNode::~DACNode() {}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq