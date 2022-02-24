/**
 * @file I2CExpanderNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/I2C9546SwitchNode.hpp"

// PDT headers
#include "ers/ers.hpp"
#include "timing/toolbox.hpp"

#include <vector>
// #include <fstream>
// #include <sstream>

namespace dunedaq {
namespace timing {

//-----------------------------------------------------------------------------
I2C9546SwitchSlave::I2C9546SwitchSlave(const I2CMasterNode* i2c_master, uint8_t address) // NOLINT(build/unsigned)
  : I2CSlave(i2c_master, address)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
I2C9546SwitchSlave::~I2C9546SwitchSlave() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2C9546SwitchSlave::ensure_valid_channel(uint8_t channel) const // NOLINT(build/unsigned)
{
  if (channel < 4)
    return;

  throw I2C9546SwitchSlaveChannelError(ERS_HERE, std::to_string(channel));
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2C9546SwitchSlave::enable_channel(uint8_t channel) const // NOLINT(build/unsigned)
{
  this->ensure_valid_channel(channel);
  std::vector<uint8_t> enable_byte = this->read_i2cPrimitive(1);
  uint8_t new_enable_byte = enable_byte.at(0) | (1UL << channel);
  this->write_i2cPrimitive({new_enable_byte});
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2C9546SwitchSlave::disable_channel(uint8_t channel) const // NOLINT(build/unsigned)
{
  this->ensure_valid_channel(channel);
  std::vector<uint8_t> enable_byte = this->read_i2cPrimitive(1);
  uint8_t new_enable_byte = enable_byte.at(0) & ~(1UL << channel);
  this->write_i2cPrimitive({new_enable_byte});
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint8_t                                               // NOLINT(build/unsigned)
I2C9546SwitchSlave::read_channels_states() const
{
  std::vector<uint8_t> enable_byte = this->read_i2cPrimitive(1);
  return enable_byte.at(0);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void                                                         // NOLINT(build/unsigned)
I2C9546SwitchSlave::set_channels_states(uint8_t channels) const // NOLINT(build/unsigned)
{ 
  this->write_i2cPrimitive({channels});
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq