/**
 * @file SIChipSlave.hpp
 *
 * SIChipSlave is a class providing an interface
 * to SI I2C devices.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_SICHIPSLAVE_HPP_
#define TIMING_INCLUDE_TIMING_SICHIPSLAVE_HPP_

#include "timing/I2CSlave.hpp"

#include "ers/Issue.hpp"

#include <map>

namespace dunedaq {
namespace timing {

/**
 * @class      SIChipSlave
 *
 * @brief      Utility class to interface to SI chips.
 * @author     Alessandro Thea
 * @date       May 2018
 */
class SIChipSlave : public I2CSlave
{
public:
  SIChipSlave(const I2CMasterNode* i2c_master, uint8_t i2c_device_address); // NOLINT(build/unsigned)
  virtual ~SIChipSlave();

  /**
   * @brief      Reads the current page.
   *
   * @return     { description_of_the_return_value }
   */
  uint8_t read_page() const; // NOLINT(build/unsigned)

  /**
   * @brief      { function_description }
   *
   * @param[in]  page  A page
   */
  void switch_page(uint8_t page) const; // NOLINT(build/unsigned)

  /**
   * @brief      Reads a device version.
   *
   * @return     { description_of_the_return_value }
   */
  uint32_t read_device_version() const; // NOLINT(build/unsigned)

  /**
   * @brief      Reads a clock register.
   *
   * @param[in]  address  A address
   *
   * @return     { description_of_the_return_value }
   */
  uint8_t read_clock_register(uint16_t address) const; // NOLINT(build/unsigned)

  /**
   * @brief      Writes a clock register.
   *
   * @param[in]  address  A address
   * @param[in]  data  A data
   */
  void write_clock_register(uint16_t address, uint8_t data) const; // NOLINT(build/unsigned)
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_SICHIPSLAVE_HPP_