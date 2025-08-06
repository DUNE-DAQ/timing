/**
 * @file LTC2945Node.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/LTC2945Node.hpp"

// PDT headers
#include "ers/ers.hpp"
#include "timing/toolbox.hpp"

#include <vector>
// #include <fstream>
// #include <sstream>

namespace dunedaq {
namespace timing {

//-----------------------------------------------------------------------------
LTC2945Node::LTC2945Node(const I2CMasterNode* i2c_master, uint8_t address, double sense_resistance) // NOLINT(build/unsigned)
  : m_sense_resistance(sense_resistance)
  , I2CSlave(i2c_master, address)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
LTC2945Node::~LTC2945Node() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/*
uint8_t
LTC2945Node::read_register(uint8_t reg_address) const
{
  // Reset bus before beginning
  reset();

  // Open the connection and send the slave address, bit 0 set to zero
  send_i2c_command_and_write_data(kStartCmd, (m_i2c_device_address << 1) & 0xfe);

  // Push the byte on the bus
  send_i2c_command_and_write_data(0x0, reg_address);

  // Open the connection & send the target i2c address. Bit 0 set to 1 (read)
  send_i2c_command_and_write_data(kStartCmd, (m_i2c_device_address << 1) | 0x01);

  uint number_of_bytes = 1;

  std::vector<uint8_t> lArray; // NOLINT(build/unsigned)
  for (unsigned ibyte = 0; ibyte < number_of_bytes; ibyte++) {

    uint8_t cmd = ((ibyte == number_of_bytes - 1) ? (kStopCmd | kAckCmd) : 0x0); // NOLINT(build/unsigned)

    // Push the cmd on the bus, retrieve the result and put it in the arrary
    lArray.push_back(send_i2c_command_and_read_data(cmd));
  }
  return lArray[0];
  
}
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double
LTC2945Node::read_v_in() const
{
  std::vector<uint8_t> v_in_bytes = this->read_i2cArray_atomic(0x1e, 0x2); // NOLINT(build/unsigned)
  uint16_t v_in_raw = combine_adc_data(v_in_bytes[0], v_in_bytes[1]);
  double v_in = v_in_raw*v_in_resolution;

  TLOG_DEBUG(13) << "LTC2945 Vin data - "
  << "raw bytes: 0x" << std::hex << (uint)v_in_bytes[0] << ", 0x" << (uint)v_in_bytes[1] // NOLINT(build/unsigned)
  << ", combined word: 0x" << v_in_raw
  << ", Vin [V]: " << v_in;
  return v_in;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double
LTC2945Node::read_delta_sense_v() const
{
  std::vector<uint8_t> v_bytes = this->read_i2cArray_atomic(0x14, 0x2); // NOLINT(build/unsigned)
  uint16_t v_raw = combine_adc_data(v_bytes[0], v_bytes[1]);
  double v = v_raw*delta_sense_v_resolution;

  TLOG_DEBUG(13) << "LTC2945 deltaSense V data - "
  << "raw bytes: 0x" << std::hex << (uint)v_bytes[0] << ", 0x" << (uint)v_bytes[1] // NOLINT(build/unsigned)
  << ", combined word: 0x" << v_raw
  << ", detlaSense V [mV]: " << v*1000;
  return v;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double
LTC2945Node::read_power() const
{
  std::vector<uint8_t> power_bytes = this->read_i2cArray_atomic(0x05, 0x3); // NOLINT(build/unsigned)
  uint32_t power_raw = combine_power_data(power_bytes[0], power_bytes[1], power_bytes[2]);
  double power = (power_raw * v_in_resolution * delta_sense_v_resolution) / m_sense_resistance;

  TLOG_DEBUG(13) << "LTC2945 power data - "
  << "raw bytes: 0x" << std::hex << (uint)power_bytes[0] << ", 0x" << (uint)power_bytes[1] << ", 0x" << (uint)power_bytes[2] // NOLINT(build/unsigned)
  << ", combined word: 0x" << power_raw
  << ", power [mW]: " << power*1000;
  return power;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint16_t
LTC2945Node::combine_adc_data(uint8_t msb_byte, uint8_t lsb_byte)
{
  uint16_t adc = (uint16_t)msb_byte << 4;
  adc = adc | (lsb_byte >> 4);
  return adc;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t
LTC2945Node::combine_power_data(uint8_t msb_byte_2, uint8_t msb_byte_1, uint8_t lsb_byte)
{
  uint32_t power = (uint32_t)msb_byte_2 << 16;
  power = power | ((uint16_t)msb_byte_1 << 8);
  power = power | lsb_byte;
  return power;
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq