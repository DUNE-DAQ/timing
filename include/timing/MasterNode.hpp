/**
 * @file MasterNode.hpp
 *
 * MasterNode is a class providing an interface
 * to the PD-I master firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_MASTERNODE_HPP_
#define TIMING_INCLUDE_TIMING_MASTERNODE_HPP_

// PDT Headers
#include "timing/definitions.hpp"
#include "timing/toolbox.hpp"
#include "timing/FLCmdGeneratorNode.hpp"
#include "timing/MasterNodeInterface.hpp"
#include "timing/MasterGlobalNode.hpp"
#include "timing/timingfirmwareinfo/InfoNljs.hpp"
#include "timing/timingfirmwareinfo/InfoStructs.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for PD-II/DUNE master timing nodes.
 */
class MasterNode : public MasterNodeInterface
{
  UHAL_DERIVEDNODE(MasterNode)
public:
  explicit MasterNode(const uhal::Node& node);
  virtual ~MasterNode();

  /**
   * @brief     Print the status of the timing node.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief     Print the status of the timing node.
   */
  std::string get_status_with_date(uint32_t clock_frequency_hz, bool print_out = false) const; // NOLINT(build/unsigned)

  /**
   * @brief     Control the tx line of endpoint sfp
   */
  void switch_endpoint_sfp(uint32_t address, bool turn_on) const override; // NOLINT(build/unsigned)

  /**
   * @brief     Enable RTT endpoint
   */
  void enable_upstream_endpoint() const override;

  /**
   * @brief     Send a fixed length command
   */
  void send_fl_cmd(FixedLengthCommandType command,
                           uint32_t channel,                                // NOLINT(build/unsigned)
                           uint32_t number_of_commands = 1) const override; // NOLINT(build/unsigned)
  
  /**
   * @brief      Measure the endpoint round trip time.
   *
   * @return     { description_of_the_return_value }
   */
  uint32_t measure_endpoint_rtt(uint32_t address, bool control_sfp = true) const override; // NOLINT(build/unsigned)

  /**
   * @brief     Apply delay to endpoint
   */
  void apply_endpoint_delay(uint32_t address,      // NOLINT(build/unsigned)
                            uint32_t coarse_delay, // NOLINT(build/unsigned)
                            uint32_t fine_delay,   // NOLINT(build/unsigned)
                            uint32_t phase_delay,  // NOLINT(build/unsigned)
                            bool measure_rtt = false,
                            bool control_sfp = true) const override;

  using MasterNodeInterface::apply_endpoint_delay;

  /**
   * @brief     Set timestamp to current machine time
   */
  void sync_timestamp(uint32_t clock_frequency_hz) const override; // NOLINT(build/unsigned)

    /**
   * @brief      Read the current timestamp word.
   *
   * @return     { description_of_the_return_value }
   */
  uint64_t read_timestamp() const override; // NOLINT(build/unsigned)

  /**
   * @brief      Set the timestamp to current time.
   */
  void set_timestamp(uint64_t timestamp) const override; // NOLINT(build/unsigned)

  /**
   * @brief     Fill the PD-I master monitoring structure.
   */
//  void get_info(timingfirmwareinfo::MasterMonitorData& mon_data) const;

  /**
   * @brief    Give info to collector.
   */
  void get_info(opmonlib::InfoCollector& ic, int level) const override;

  /**
   * @brief    Read some data from endpoint registers
   */
  void reset_command_counters() const;

  /**
   * @brief    Send an async packet
   */
  std::vector<uint32_t> transmit_async_packet(const std::vector<uint32_t>& packet, int timeout=500) const;

  /**
   * @brief    Write some data to endpoint registers
   */
  void write_endpoint_data(uint16_t endpoint_address, uint8_t reg_address, std::vector<uint8_t> data, bool address_mode) const;

  /**
   * @brief    Read some data from endpoint registers
   */
  std::vector<uint32_t> read_endpoint_data(uint16_t endpoint_address, uint8_t reg_address, uint8_t data_length, bool address_mode) const;

  /**
   * @brief    Disable timestamp sending
   */
  void disable_timestamp_broadcast() const;

  /**
   * @brief    Enable timestamp sending
   */
  void enable_timestamp_broadcast() const;

};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_PDIMASTERNODE_HPP_