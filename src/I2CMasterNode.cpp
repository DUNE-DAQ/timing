/**
 * @file I2CMasterNode.cpp
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

#include "timing/I2CMasterNode.hpp"

#include "ers/ers.hpp"
#include "timing/I2CSlave.hpp"
#include "timing/TimingIssues.hpp"
#include "timing/toolbox.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>

#include <algorithm>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(I2CMasterNode)

// PRIVATE CONST definitions
const std::string I2CMasterNode::kPreHiNode = "ps_hi";
const std::string I2CMasterNode::kPreLoNode = "ps_lo";
const std::string I2CMasterNode::kCtrlNode = "ctrl";
const std::string I2CMasterNode::kTxNode = "data";
const std::string I2CMasterNode::kRxNode = "data";
const std::string I2CMasterNode::kCmdNode = "cmd_stat";
const std::string I2CMasterNode::kStatusNode = "cmd_stat";

const uint8_t I2CMasterNode::kStartCmd = 0x80;         // 1 << 7 // NOLINT(build/unsigned)
const uint8_t I2CMasterNode::kStopCmd = 0x40;          // 1 << 6 // NOLINT(build/unsigned)
const uint8_t I2CMasterNode::kReadFromSlaveCmd = 0x20; // 1 << 5 // NOLINT(build/unsigned)
const uint8_t I2CMasterNode::kWriteToSlaveCmd = 0x10;  // 1 << 4 // NOLINT(build/unsigned)
const uint8_t I2CMasterNode::kAckCmd = 0x08;           // 1 << 3 // NOLINT(build/unsigned)
const uint8_t I2CMasterNode::kInterruptAck = 0x01;     // 1      // NOLINT(build/unsigned)

const uint8_t I2CMasterNode::kReceivedAckBit = 0x80;     // recvdack = 0x1 << 7   // NOLINT(build/unsigned)
const uint8_t I2CMasterNode::kBusyBit = 0x40;            // busy = 0x1 << 6       // NOLINT(build/unsigned)
const uint8_t I2CMasterNode::kArbitrationLostBit = 0x20; // arblost = 0x1 << 5    // NOLINT(build/unsigned)
const uint8_t I2CMasterNode::kInProgressBit = 0x2;       // inprogress = 0x1 << 1 // NOLINT(build/unsigned)
const uint8_t I2CMasterNode::kInterruptBit = 0x1;        // interrupt = 0x1       // NOLINT(build/unsigned)

//-----------------------------------------------------------------------------
I2CMasterNode::I2CMasterNode(const uhal::Node& node)
  : uhal::Node(node)
{
  constructor();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
I2CMasterNode::I2CMasterNode(const I2CMasterNode& node)
  : uhal::Node(node)
{
  constructor();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CMasterNode::constructor()
{
  // 16 bit clock prescale factor.
  // formula: m_clockPrescale = (input_frequency / 5 / desired_frequency) -1
  // for typical IPbus applications: input frequency = IPbus clock = 31.x MHz
  // target frequency 100 kHz to play it safe (first revision of i2c standard),
  m_clock_prescale = 0x40;
  // m_clock_prescale = 0x100;

  // Build the list of slaves
  // Loop over node parameters. Each parameter becomes a slave node.
  const std::unordered_map<std::string, std::string>& parameters = this->getParameters();
  std::unordered_map<std::string, std::string>::const_iterator it;
  for (it = parameters.begin(); it != parameters.end(); ++it) {
    uint32_t slave_addr = (boost::lexical_cast<timing::stoul<uint32_t>>(it->second) & 0x7f); // NOLINT(build/unsigned)
    m_i2c_device_addresses.insert(std::make_pair(it->first, slave_addr));
    m_i2c_devices.insert(std::make_pair(it->first, new I2CSlave(this, slave_addr)));
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
I2CMasterNode::~I2CMasterNode()
{
  std::unordered_map<std::string, I2CSlave*>::iterator it;
  for (it = m_i2c_devices.begin(); it != m_i2c_devices.end(); ++it) {
    // Delete slaves
    delete it->second; // NOLINT
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::vector<std::string>
I2CMasterNode::get_slaves() const
{
  std::vector<std::string> slaves;

  boost::copy(m_i2c_device_addresses | boost::adaptors::map_keys, std::back_inserter(slaves));
  return slaves;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint8_t // NOLINT(build/unsigned)
I2CMasterNode::get_slave_address(const std::string& name) const
{
  std::unordered_map<std::string, uint8_t>::const_iterator it = // NOLINT(build/unsigned)
    m_i2c_device_addresses.find(name);
  if (it == m_i2c_device_addresses.end()) {
    throw I2CDeviceNotFound(ERS_HERE, getId(), name);
  }
  return it->second;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
const I2CSlave&
I2CMasterNode::get_slave(const std::string& name) const
{
  std::unordered_map<std::string, I2CSlave*>::const_iterator it = m_i2c_devices.find(name);
  if (it == m_i2c_devices.end()) {
    throw I2CDeviceNotFound(ERS_HERE, getId(), name);
  }
  return *(it->second);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint8_t                                                                             // NOLINT(build/unsigned)
I2CMasterNode::read_i2c(uint8_t i2c_device_address, uint32_t i2c_reg_address) const // NOLINT(build/unsigned)
{
  // // write one word containing the address
  // std::vector<uint8_t> array(1, i2c_reg_address & 0x7f);
  // this->write_block_i2c(i2c_device_address, array);
  // // request the content at the specific address
  // return this->read_block_i2c(i2c_device_address, 1) [0];
  return this->read_i2cArray(i2c_device_address, i2c_reg_address, 1)[0];
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CMasterNode::write_i2c(uint8_t i2c_device_address, // NOLINT(build/unsigned)
                         uint32_t i2c_reg_address,   // NOLINT(build/unsigned)
                         uint8_t data,               // NOLINT(build/unsigned)
                         bool send_stop) const
{
  // std::vector<uint8_t> block(2);
  // block[0] = (i2c_reg_address & 0xff);
  // block[1] = (data & 0xff);
  // this->write_block_i2c(i2c_device_address, block);

  this->write_i2cArray(i2c_device_address, i2c_reg_address, { data }, send_stop);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::vector<uint8_t>                                         // NOLINT(build/unsigned)
I2CMasterNode::read_i2cArray(uint8_t i2c_device_address,     // NOLINT(build/unsigned)
                             uint32_t i2c_reg_address,       // NOLINT(build/unsigned)
                             uint32_t number_of_words) const // NOLINT(build/unsigned)
{
  // write one word containing the address
  std::vector<uint8_t> lArray{ (uint8_t)(i2c_reg_address & 0xff) }; // NOLINT(build/unsigned)
  this->write_block_i2c(i2c_device_address, lArray);
  // request the content at the specific address
  return this->read_block_i2c(i2c_device_address, number_of_words);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CMasterNode::write_i2cArray(uint8_t i2c_device_address, // NOLINT(build/unsigned)
                              uint32_t i2c_reg_address,   // NOLINT(build/unsigned)
                              std::vector<uint8_t> data,  // NOLINT(build/unsigned)
                              bool send_stop) const
{
  // std::cout << "Writing " << data.size() << " from " << std::showbase <<  std::hex << i2c_reg_address << " on " <<
  // (uint32_t)i2c_device_address << std::endl; // HACK
  std::vector<uint8_t> block(data.size() + 1); // NOLINT(build/unsigned)
  block[0] = (i2c_reg_address & 0xff);

  for (size_t i(0); i < data.size(); ++i)
    block[i + 1] = data[i];

  this->write_block_i2c(i2c_device_address, block, send_stop);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::vector<uint8_t>                                                                         // NOLINT(build/unsigned)
I2CMasterNode::read_i2cPrimitive(uint8_t i2c_device_address, uint32_t number_of_bytes) const // NOLINT(build/unsigned)
{
  return this->read_block_i2c(i2c_device_address, number_of_bytes);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CMasterNode::write_i2cPrimitive(uint8_t i2c_device_address,       // NOLINT(build/unsigned)
                                  const std::vector<uint8_t>& data, // NOLINT(build/unsigned)
                                  bool send_stop) const
{
  this->write_block_i2c(i2c_device_address, data, send_stop);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CMasterNode::write_block_i2c(uint8_t i2c_device_address,         // NOLINT(build/unsigned)
                               const std::vector<uint8_t>& data, // NOLINT(build/unsigned)
                               bool send_stop) const
{
  // transmit reg definitions
  // bits 7-1: 7-bit slave address during address transfer
  //           or first 7 bits of byte during data transfer
  // bit 0: RW flag during address transfer or LSB during data transfer.
  // '1' = reading from slave
  // '0' = writing to slave
  // command reg definitions
  // bit 7: Generate start condition
  // bit 6: Generate stop condition
  // bit 5: Read from slave
  // bit 4: Write to slave
  // bit 3: 0 when acknowledgement is received
  // bit 2:1: Reserved
  // bit 0: Interrupt acknowledge. When set, clears a pending interrupt

  // Reset bus before beginning
  reset();

  // Open the connection and send the slave address, bit 0 set to zero
  send_i2c_command_and_write_data(kStartCmd, (i2c_device_address << 1) & 0xfe);

  for (unsigned ibyte = 0; ibyte < data.size(); ibyte++) {

    // Send stop if last element of the array (and not vetoed)
    uint8_t cmd = (((ibyte == data.size() - 1) && send_stop) ? kStopCmd : 0x0); // NOLINT(build/unsigned)

    // Push the byte on the bus
    send_i2c_command_and_write_data(cmd, data[ibyte]);
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::vector<uint8_t>                                                                // NOLINT(build/unsigned)
I2CMasterNode::read_block_i2c(uint8_t i2c_device_address, uint32_t number_of_bytes) const // NOLINT(build/unsigned)
{
  // transmit reg definitions
  // bits 7-1: 7-bit slave address during address transfer
  //           or first 7 bits of byte during data transfer
  // bit 0: RW flag during address transfer or LSB during data transfer.
  //        '1' = reading from slave
  //        '0' = writing to slave
  // command reg definitions
  // bit 7:   Generate start condition
  // bit 6:   Generate stop condition
  // bit 5:   Read from slave
  // bit 4:   Write to slave
  // bit 3:   0 when acknowledgement is received
  // bit 2:1: Reserved
  // bit 0:   Interrupt acknowledge. When set, clears a pending interrupt

  // Reset bus before beginning
  reset();

  // Open the connection & send the target i2c address. Bit 0 set to 1 (read)
  send_i2c_command_and_write_data(kStartCmd, (i2c_device_address << 1) | 0x01);

  std::vector<uint8_t> lArray; // NOLINT(build/unsigned)
  for (unsigned ibyte = 0; ibyte < number_of_bytes; ibyte++) {

    uint8_t cmd = ((ibyte == number_of_bytes - 1) ? (kStopCmd | kAckCmd) : 0x0); // NOLINT(build/unsigned)

    // Push the cmd on the bus, retrieve the result and put it in the arrary
    lArray.push_back(send_i2c_command_and_read_data(cmd));
  }
  return lArray;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
I2CMasterNode::ping(uint8_t i2c_device_address) const // NOLINT(build/unsigned)
{
  // Reset bus before beginning
  reset();

  try {
    send_i2c_command_and_write_data(kStartCmd, (i2c_device_address << 1) | 0x01);
    send_i2c_command_and_read_data(kStopCmd | kAckCmd);
    return true;
  } catch (const timing::I2CException& excp) {
    return false;
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::vector<uint8_t> // NOLINT(build/unsigned)
I2CMasterNode::scan() const
{

  std::vector<uint8_t> address_vector; // NOLINT(build/unsigned)

  // Reset bus before beginning
  reset();

  for (uint8_t iaddr(0); iaddr < 0x7f; ++iaddr) { // NOLINT(build/unsigned)
    // Open the connection & send the target i2c address. Bit 0 set to 1 (read)
    try {
      send_i2c_command_and_write_data(kStartCmd, (iaddr << 1) | 0x01);
      send_i2c_command_and_read_data(kStopCmd | kAckCmd);
    } catch (const timing::I2CException& excp) {
      // TIMING_LOG(kError) << std::showbase << std::hex << (uint32_t)iaddr << "  " << excp.what();
      continue;
    }
    address_vector.push_back(iaddr);
    // TIMING_LOG(kNotice) << std::showbase << std::hex << (uint32_t)iaddr << " found";
  }

  return address_vector;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CMasterNode::reset() const
{
  // Resets the I2C bus
  //
  // This function does the following:
  //        1) Disables the I2C core
  //        2) Sets the clock prescale registers
  //        3) Enables the I2C core
  //        4) Sets all writable bus-master registers to default values

  auto ctrl = getNode(kCtrlNode).read();
  auto pre_hi = getNode(kPreHiNode).read();
  auto pre_lo = getNode(kPreLoNode).read();
  getClient().dispatch();

  bool full_reset(false);

  full_reset = (m_clock_prescale != (pre_hi << 8) + pre_lo);

  if (full_reset) {
    // disable the I2C core
    getNode(kCtrlNode).write(0x00);
    getClient().dispatch();
    // set the clock prescale
    getNode(kPreHiNode).write((m_clock_prescale & 0xff00) >> 8);
    // getClient().dispatch();
    getNode(kPreLoNode).write(m_clock_prescale & 0xff);
    // getClient().dispatch();
    // set all writable bus-master registers to default values
    getNode(kTxNode).write(0x00);
    getNode(kCmdNode).write(0x00);
    getClient().dispatch();

    // enable the I2C core
    getNode(kCtrlNode).write(0x80);
    getClient().dispatch();
  } else {
    // set all writable bus-master registers to default values
    getNode(kTxNode).write(0x00);
    getNode(kCmdNode).write(0x00);
    getClient().dispatch();
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint8_t                                                              // NOLINT(build/unsigned)
I2CMasterNode::send_i2c_command_and_read_data(uint8_t command) const // NOLINT(build/unsigned)
{

  assert(!(command & kWriteToSlaveCmd));

  uint8_t full_cmd = command | kReadFromSlaveCmd;                                     // NOLINT(build/unsigned)
  TLOG_DEBUG(10) << ">> sending read cmd  = " << format_reg_value((uint32_t)full_cmd); // NOLINT(build/unsigned)

  // Force the read bit high and set them cmd bits
  getNode(kCmdNode).write(full_cmd);
  getClient().dispatch();

  // Wait for transaction to finish. Require idle bus at the end if stop bit is high)
  wait_until_finished(/*req ack*/ false, command & kStopCmd);

  // Pull the data out of the rx register.
  uhal::ValWord<uint32_t> result = getNode(kRxNode).read(); // NOLINT(build/unsigned)
  getClient().dispatch();

  TLOG_DEBUG(10) << "<< receive data      = " << format_reg_value((uint32_t)result); // NOLINT(build/unsigned)v

  return (result & 0xff);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CMasterNode::send_i2c_command_and_write_data(uint8_t command, uint8_t data) const // NOLINT(build/unsigned)
{

  //
  assert(!(command & kReadFromSlaveCmd));

  uint8_t full_cmd = command | kWriteToSlaveCmd; // NOLINT(build/unsigned)
  std::stringstream debug_stream;
  debug_stream << ">> sending write cmd = " << std::showbase << std::hex << (uint32_t)full_cmd // NOLINT(build/unsigned)
               << " data = " << std::showbase << std::hex << (uint32_t)data;                   // NOLINT(build/unsigned)
  TLOG_DEBUG(10) << debug_stream.str();

  // write the payload
  getNode(kTxNode).write(data);
  getClient().dispatch();

  // Force the write bit high and set them cmd bits
  getNode(kCmdNode).write(full_cmd);

  // Run the commands and wait for transaction to finish
  getClient().dispatch();

  // Wait for transaction to finish. Require idle bus at the end if stop bit is high
  // wait_until_finished(req_hack, requ_idle)
  wait_until_finished( true, command & kStopCmd); // NOLINT
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CMasterNode::wait_until_finished(bool require_acknowledgement, bool require_bus_idle_at_end) const
{
  // Ensures the current bus transaction has finished successfully
  // before allowing further I2C bus transactions
  // This method monitors the status register
  // and will not allow execution to continue until the
  // I2C bus has completed properly.  It will throw an exception
  // if it picks up bus problems or a bus timeout occurs.
  const unsigned max_retry = 20;
  unsigned attempt = 1;
  bool received_acknowledge, busy;

  const uhal::Node& status_node = getNode(kStatusNode);

  while (attempt <= max_retry) {
    usleep(10);
    // Get the status
    uhal::ValWord<uint32_t> i2c_status = status_node.read(); // NOLINT(build/unsigned)
    getClient().dispatch();

    received_acknowledge = !(i2c_status & kReceivedAckBit);
    busy = (i2c_status & kBusyBit);
    bool arbitration_lost = (i2c_status & kArbitrationLostBit);
    bool transfer_in_progress = (i2c_status & kInProgressBit);

    if (arbitration_lost) {
      // This is an instant error at any time
      throw I2CBusArbitrationLost(ERS_HERE, getId());
    }

    if (!transfer_in_progress) {
      // The transfer looks to have completed successfully,
      // pending further checks
      break;
    }

    attempt += 1;
  }

  // At this point, we've either had too many retries, or the
  // Transfer in Progress (TIP) bit went low.  If the TIP bit
  // did go low, then we do a couple of other checks to see if
  // the bus operated as expected:

  if (attempt > max_retry) {
    throw I2CTransactionTimeout(ERS_HERE, getId());
  }

  if (require_acknowledgement && !received_acknowledge) {
    throw I2CNoAcknowledgeReceived(ERS_HERE, getId());
  }

  if (require_bus_idle_at_end && busy) {
    throw I2CTransferFinishedBusStillBusy(ERS_HERE, getId());
  }
}

} // namespace timing
} // namespace dunedaq