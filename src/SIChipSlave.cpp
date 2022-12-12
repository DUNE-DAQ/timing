/**
 * @file SIChipSlave.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/SIChipSlave.hpp"

// PDT headers
#include "ers/ers.hpp"
#include "timing/toolbox.hpp"

#include <boost/tuple/tuple.hpp>

#include <fstream>
#include <sstream>

namespace dunedaq {
namespace timing {

//-----------------------------------------------------------------------------
SIChipSlave::SIChipSlave(const I2CMasterNode* i2c_master, uint8_t address) // NOLINT(build/unsigned)
  : I2CSlave(i2c_master, address)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
SIChipSlave::~SIChipSlave() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint8_t // NOLINT(build/unsigned)
SIChipSlave::read_page() const
{

  TLOG_DEBUG(7) << "<- Reading page ";

  // Read from the page address (0x1?)
  return read_i2c(0x1);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SIChipSlave::switch_page(uint8_t page) const // NOLINT(build/unsigned)
{

  // Prepare a data block with address and new page
  // std::vector<uint8_t> lData = {0x1, page};// NOLINT(build/unsigned)
  TLOG_DEBUG(7) << "-> Switching to page " << format_reg_value((uint32_t)page); // NOLINT(build/unsigned)
  write_i2c(0x1, page);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
SIChipSlave::read_device_version() const
{

  // Go to the right page
  switch_page(0x0);
  // Read 2 words from 0x2
  auto version = read_i2cArray(0x2, 2);

  return (((uint32_t)version[1] << 8) + (uint32_t)version[0]); // NOLINT(build/unsigned)
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint8_t                                                  // NOLINT(build/unsigned)
SIChipSlave::read_clock_register(uint16_t address) const // NOLINT(build/unsigned)
{

  uint8_t reg_address = (address & 0xff);       // NOLINT(build/unsigned)
  uint8_t page_address = (address >> 8) & 0xff; // NOLINT(build/unsigned)
  std::stringstream debug_stream;
  debug_stream << std::showbase << std::hex << "Read Address " << (uint32_t)address // NOLINT(build/unsigned)
               << " reg: " << (uint32_t)reg_address                                    // NOLINT(build/unsigned)
               << " page: " << (uint32_t)page_address;                                 // NOLINT(build/unsigned)
  TLOG_DEBUG(6) << debug_stream.str();
  // Change page only when required.
  // (The SI5344 don't like to have the page register id to be written all the time.)
  uint8_t current_address = read_page(); // NOLINT(build/unsigned)
  if (page_address != current_address) {
    switch_page(page_address);
  }

  // Read the register
  return read_i2c(reg_address);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SIChipSlave::write_clock_register(uint16_t address, uint8_t data) const // NOLINT(build/unsigned)
{

  uint8_t reg_address = (address & 0xff);       // NOLINT(build/unsigned)
  uint8_t page_address = (address >> 8) & 0xff; // NOLINT(build/unsigned)

  std::stringstream debug_stream;
  debug_stream << std::showbase << std::hex << "Write Address " << (uint32_t)address // NOLINT(build/unsigned)
               << " reg: " << (uint32_t)reg_address                                     // NOLINT(build/unsigned)
               << " page: " << (uint32_t)page_address;                                  // NOLINT(build/unsigned)
  TLOG_DEBUG(6) << debug_stream.str();
  // Change page only when required.
  // (The SI5344 don't like to have the page register id to be written all the time.)
  uint8_t current_address = read_page(); // NOLINT(build/unsigned)
  if (page_address != current_address) {
    switch_page(page_address);
  }

  return write_i2c(reg_address, data);
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq