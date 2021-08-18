/**
 * @file I2CMasterNode.hpp
 * @class      I2CMasterNode
 *
 * @brief      OpenCode I2C interface to a ipbus node
 * @author     Kristian Harder, Alessandro Thea
 * @date       August 2013
 *
 * The class is non-copyable on purpose as the inheriting object
 * must properly set the node pointer in the copy i2c access through
 * an IPbus interface.
 *
 * I2CMasterNode is a class providing an interface
 * to the I2C master firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_I2CMASTERNODE_HPP_
#define TIMING_INCLUDE_TIMING_I2CMASTERNODE_HPP_

#include "timing/TimingNode.hpp"

// uHal Headers
#include "TimingIssues.hpp"
#include "ers/Issue.hpp"
#include "uhal/DerivedNode.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace dunedaq {
namespace timing {

class I2CSlave;

class I2CMasterNode : public uhal::Node
{
  UHAL_DERIVEDNODE(I2CMasterNode)
public:
  explicit I2CMasterNode(const uhal::Node& node);
  I2CMasterNode(const I2CMasterNode& node);
  virtual ~I2CMasterNode();

  ///
  virtual uint16_t get_i2c_clock_prescale() const { return m_clock_prescale; } // NOLINT(build/unsigned)

  virtual std::vector<std::string> get_slaves() const;
  virtual uint8_t get_slave_address(const std::string& name) const; // NOLINT(build/unsigned)
  virtual const I2CSlave& get_slave(const std::string& name) const;

  void reset() const;

  /// commodity functions
  virtual uint8_t read_i2c(uint8_t i2c_device_address, uint32_t i2c_reg_address) const; // NOLINT(build/unsigned)
  virtual void write_i2c(uint8_t i2c_device_address,                                    // NOLINT(build/unsigned)
                         uint32_t i2c_reg_address,                                      // NOLINT(build/unsigned)
                         uint8_t data,                                                  // NOLINT(build/unsigned)
                         bool send_stop = true) const;

  virtual std::vector<uint8_t> read_i2cArray(uint8_t i2c_device_address,      // NOLINT(build/unsigned)
                                             uint32_t i2c_reg_address,        // NOLINT(build/unsigned)
                                             uint32_t number_of_words) const; // NOLINT(build/unsigned)
  virtual void write_i2cArray(uint8_t i2c_device_address,                     // NOLINT(build/unsigned)
                              uint32_t i2c_reg_address,                       // NOLINT(build/unsigned)
                              std::vector<uint8_t> data,                      // NOLINT(build/unsigned)
                              bool send_stop = true) const;

  virtual std::vector<uint8_t> read_i2cPrimitive(uint8_t i2c_device_address,      // NOLINT(build/unsigned)
                                                 uint32_t number_of_bytes) const; // NOLINT(build/unsigned)
  virtual void write_i2cPrimitive(uint8_t i2c_device_address,                     // NOLINT(build/unsigned)
                                  const std::vector<uint8_t>& data,               // NOLINT(build/unsigned)
                                  bool send_stop = true) const;

  bool ping(uint8_t i2c_device_address) const; // NOLINT(build/unsigned)

  std::vector<uint8_t> scan() const; // NOLINT(build/unsigned)

protected:
  // low level i2c functions
  std::vector<uint8_t> virtual read_block_i2c(uint8_t i2c_device_address,      // NOLINT(build/unsigned)
                                              uint32_t number_of_bytes) const; // NOLINT(build/unsigned)
  void virtual write_block_i2c(uint8_t i2c_device_address,                     // NOLINT(build/unsigned)
                               const std::vector<uint8_t>& data,               // NOLINT(build/unsigned)
                               bool send_stop = true) const;

  uint8_t send_i2c_command_and_read_data(uint8_t command) const;             // NOLINT(build/unsigned)
  void send_i2c_command_and_write_data(uint8_t command, uint8_t data) const; // NOLINT(build/unsigned)

  //! Slaves
  std::unordered_map<std::string, uint8_t> m_i2c_device_addresses; // NOLINT(build/unsigned)

private:
  ///
  void constructor();

  // low level i2c functions
  void wait_until_finished(bool require_acknowledgement = true, bool require_bus_idle_at_end = false) const;

  //! IPBus register names for i2c bus
  static const std::string kPreHiNode;
  static const std::string kPreLoNode;
  static const std::string kCtrlNode;
  static const std::string kTxNode;
  static const std::string kRxNode;
  static const std::string kCmdNode;
  static const std::string kStatusNode;

  static const uint8_t kStartCmd;         // 1 << 7 // NOLINT(build/unsigned)
  static const uint8_t kStopCmd;          // 1 <<   // NOLINT(build/unsigned)
  static const uint8_t kReadFromSlaveCmd; // 1 << 5 // NOLINT(build/unsigned)
  static const uint8_t kWriteToSlaveCmd;  // 1 << 4 // NOLINT(build/unsigned)
  static const uint8_t kAckCmd;           // 1 << 3 // NOLINT(build/unsigned)
  static const uint8_t kInterruptAck;     // 1      // NOLINT(build/unsigned)

  static const uint8_t kReceivedAckBit;     // recvdack = 0x1 << 7   // NOLINT(build/unsigned)
  static const uint8_t kBusyBit;            // busy = 0x1 << 6       // NOLINT(build/unsigned)
  static const uint8_t kArbitrationLostBit; // arblost = 0x1 << 5    // NOLINT(build/unsigned)
  static const uint8_t kInProgressBit;      // inprogress = 0x1 << 1 // NOLINT(build/unsigned)
  static const uint8_t kInterruptBit;       // interrupt = 0x1       // NOLINT(build/unsigned)

  //! clock prescale factor
  uint16_t m_clock_prescale; // NOLINT(build/unsigned)

  //! I2C slaves attached to this node
  std::unordered_map<std::string, I2CSlave*>
    m_i2c_devices; // TODO, Eric Flumerfelt <eflumerf@fnal.gov> May-21-2021: Consider using smart pointers

  friend class I2CSlave;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_I2CMASTERNODE_HPP_
