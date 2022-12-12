/**
 * @file I2CSlave.cpp
 *
 * File:   OpenCoresI2CMasterNode.cpp
 * Author: ale
 *
 * Created on August 29, 2014, 4:47 PM
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/I2CSlave.hpp"

#include "ers/ers.hpp"
#include "timing/I2CMasterNode.hpp"
#include "timing/TimingIssues.hpp"
#include "timing/toolbox.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/unordered/unordered_map.hpp>

#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

//-----------------------------------------------------------------------------
I2CSlave::I2CSlave(const I2CMasterNode* i2c_master, uint8_t i2c_device_address) // NOLINT(build/unsigned)
  : m_i2c_master(i2c_master)
  , m_i2c_device_address(i2c_device_address)
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
I2CSlave::~I2CSlave() {}
//-----------------------------------------------------------------------------

// comodity functions
//-----------------------------------------------------------------------------
uint8_t                                                                         // NOLINT(build/unsigned)
I2CSlave::read_i2c(uint32_t i2c_device_address, uint32_t i2c_reg_address) const // NOLINT(build/unsigned)
{
  return m_i2c_master->read_i2c(i2c_device_address, i2c_reg_address);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint8_t                                            // NOLINT(build/unsigned)
I2CSlave::read_i2c(uint32_t i2c_reg_address) const // NOLINT(build/unsigned)
{
  return m_i2c_master->read_i2c(m_i2c_device_address, i2c_reg_address);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CSlave::write_i2c(uint32_t i2c_device_address, // NOLINT(build/unsigned)
                    uint32_t i2c_reg_address,    // NOLINT(build/unsigned)
                    uint8_t data,                // NOLINT(build/unsigned)
                    bool send_stop) const
{
  m_i2c_master->write_i2c(i2c_device_address, i2c_reg_address, data, send_stop);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CSlave::write_i2c(uint32_t i2c_reg_address, uint8_t data, bool send_stop) const // NOLINT(build/unsigned)
{
  m_i2c_master->write_i2c(m_i2c_device_address, i2c_reg_address, data, send_stop);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::vector<uint8_t>                                    // NOLINT(build/unsigned)
I2CSlave::read_i2cArray(uint32_t i2c_device_address,    // NOLINT(build/unsigned)
                        uint32_t i2c_reg_address,       // NOLINT(build/unsigned)
                        uint32_t number_of_words) const // NOLINT(build/unsigned)
{
  return m_i2c_master->read_i2cArray(i2c_device_address, i2c_reg_address, number_of_words);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::vector<uint8_t>                                                              // NOLINT(build/unsigned)
I2CSlave::read_i2cArray(uint32_t i2c_reg_address, uint32_t number_of_words) const // NOLINT(build/unsigned)
{
  return m_i2c_master->read_i2cArray(m_i2c_device_address, i2c_reg_address, number_of_words);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CSlave::write_i2cArray(uint32_t i2c_device_address, // NOLINT(build/unsigned)
                         uint32_t i2c_reg_address,    // NOLINT(build/unsigned)
                         std::vector<uint8_t> data,   // NOLINT(build/unsigned)
                         bool send_stop) const
{
  m_i2c_master->write_i2cArray(i2c_device_address, i2c_reg_address, data, send_stop);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CSlave::write_i2cArray(uint32_t i2c_reg_address,  // NOLINT(build/unsigned)
                         std::vector<uint8_t> data, // NOLINT(build/unsigned)
                         bool send_stop) const
{
  m_i2c_master->write_i2cArray(m_i2c_device_address, i2c_reg_address, data, send_stop);
}
//-----------------------------------------------------------------------------

// comodity functions
//-----------------------------------------------------------------------------
std::vector<uint8_t>                                        // NOLINT(build/unsigned)
I2CSlave::read_i2cPrimitive(uint32_t number_of_bytes) const // NOLINT(build/unsigned)
{
  return m_i2c_master->read_i2cPrimitive(m_i2c_device_address, number_of_bytes);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CSlave::write_i2cPrimitive(const std::vector<uint8_t>& data, bool send_stop) const // NOLINT(build/unsigned)
{
  m_i2c_master->write_i2cPrimitive(m_i2c_device_address, data, send_stop);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
I2CSlave::ping() const
{
  return m_i2c_master->ping(m_i2c_device_address);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
I2CSlave::get_master_id() const
{
  return m_i2c_master->getId();
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq