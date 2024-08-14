/**
 * @file I2CSlave.hpp
 *
 * I2CSlave is a base class providing an interface
 * to I2C devices.
 *
 * @class I2CSlave
 * @brief Class to provide OpenCode I2C interface to a ipbus node
 *
 * The class is non copyable on purpose as the inheriting object
 * must properly set the node pointer in the copy
 * i2c access through an IPbus interface
 * @author Kristian Harder, Alessandro Thea
 * @date August 2013
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_I2CSLAVE_HPP_
#define TIMING_INCLUDE_TIMING_I2CSLAVE_HPP_

#include "TimingIssues.hpp"

#include "ers/Issue.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

#include <boost/core/noncopyable.hpp>

#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

class I2CMasterNode;

class I2CSlave : boost::noncopyable
{

public:
  virtual ~I2CSlave();

  uint8_t get_i2c_address() const { return m_i2c_device_address; } // NOLINT(build/unsigned)

  /// comodity functions
  uint8_t read_i2c(uint32_t i2c_device_address, uint32_t i2c_reg_address) const; // NOLINT(build/unsigned)
  uint8_t read_i2c(uint32_t i2c_reg_address) const;                              // NOLINT(build/unsigned)

  void write_i2c(uint32_t i2c_device_address, // NOLINT(build/unsigned)
                 uint32_t i2c_reg_address,    // NOLINT(build/unsigned)
                 uint8_t data,                // NOLINT(build/unsigned)
                 bool send_stop = true) const;
  void write_i2c(uint32_t i2c_reg_address, uint8_t data, bool send_stop = true) const; // NOLINT(build/unsigned)

  std::vector<uint8_t> read_i2cArray(uint32_t i2c_device_address,     // NOLINT(build/unsigned)
                                     uint32_t i2c_reg_address,        // NOLINT(build/unsigned)
                                     uint32_t number_of_words) const; // NOLINT(build/unsigned)
  std::vector<uint8_t> read_i2cArray(uint32_t i2c_reg_address,        // NOLINT(build/unsigned)
                                     uint32_t number_of_words) const; // NOLINT(build/unsigned)

  void write_i2cArray(uint32_t i2c_device_address, // NOLINT(build/unsigned)
                      uint32_t i2c_reg_address,    // NOLINT(build/unsigned)
                      std::vector<uint8_t> data,   // NOLINT(build/unsigned)
                      bool send_stop = true) const;
  void write_i2cArray(uint32_t i2c_reg_address,  // NOLINT(build/unsigned)
                      std::vector<uint8_t> data, // NOLINT(build/unsigned)
                      bool send_stop = true) const;

  std::vector<uint8_t> read_i2cPrimitive(uint32_t number_of_bytes) const;                 // NOLINT(build/unsigned)
  void write_i2cPrimitive(const std::vector<uint8_t>& data, bool send_stop = true) const; // NOLINT(build/unsigned)

  bool ping() const;

  std::string get_master_id() const;

protected:
  // Private constructor, accessible to I2CMaster
  I2CSlave(const I2CMasterNode* i2c_master, uint8_t i2c_device_address); // NOLINT(build/unsigned)

private:
  const I2CMasterNode* m_i2c_master;

  // slave address
  uint8_t m_i2c_device_address; // NOLINT(build/unsigned)

  friend class I2CMasterNode;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_I2CSLAVE_HPP_
