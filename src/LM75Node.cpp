/**
 * @file I2CExpanderNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/LM75Node.hpp"

// PDT headers
#include "ers/ers.hpp"
#include "timing/toolbox.hpp"

#include <vector>
// #include <fstream>
// #include <sstream>

namespace dunedaq {
namespace timing {

//-----------------------------------------------------------------------------
LM75Node::LM75Node(const I2CMasterNode* i2c_master, uint8_t address) // NOLINT(build/unsigned)
  : I2CSlave(i2c_master, address)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
LM75Node::~LM75Node() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
float
LM75Node::read_temperature() const
{
  std::vector<uint8_t> temp_bytes = this->read_i2cPrimitive(2); // NOLINT(build/unsigned)
  uint16_t temp_raw = (temp_bytes[1] & 0x80) >> 7;
  temp_raw = temp_raw | (temp_bytes[0] << 1);

  // 1s padding for negative values
  if (temp_raw & 0x0100)
  {
    temp_raw = temp_raw | 0xfe00; 
  }
  int16_t temp = static_cast<int16_t>(temp_raw);

  TLOG_DEBUG(13) << "LM75 temp data - "
  << "raw bytes: 0x" << std::hex << (uint)temp_bytes[0] << ", 0x" << (uint)temp_bytes[1] // NOLINT(build/unsigned)
  << ", combined word: 0x" << temp_raw
  << ", temp [C]: " << temp*0.5;
  return temp*0.5;
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq