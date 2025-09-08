/**
 * @file MasterDesign.hpp
 *
 * MasterDesign is a base class providing an interface
 * to for top level master firmware designs.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_MASTERDESIGN_HPP_
#define TIMING_INCLUDE_TIMING_MASTERDESIGN_HPP_

// PDT Headers
#include "timing/TopDesign.hpp"
#include "timing/MasterDesignInterface.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <sstream>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Base class for timing master designs.
 */
class MasterDesign : public TopDesign, virtual public MasterDesignInterface
{
  UHAL_DERIVEDNODE(MasterDesign)
public:
  explicit MasterDesign(const uhal::Node& node);
  virtual ~MasterDesign();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief      Prepare the timing master for data taking.
   *
   */
  void configure(ClockSource clock_source, TimestampSource ts_source) const override;
  
  /**
   * @brief      Read the current timestamp.
   *
   * @return     { description_of_the_return_value }
   */
  uint64_t read_master_timestamp() const override; // NOLINT(build/unsigned)

  /**
   * @brief      Sync timestamp to current machine value.
   *
   */
  void sync_timestamp(TimestampSource source) const override;
  
  /**
   * @brief      Measure the endpoint round trip time.
   *
   * @return     { description_of_the_return_value }
   */
  uint32_t measure_endpoint_rtt(uint16_t address, // NOLINT(build/unsigned)
                                        uint16_t fanout_endpoint_address,
                                        uint8_t fanout_mux,
                                        bool control_sfp = true) const override;

  uint32_t measure_endpoint_rtt(uint16_t address, // NOLINT(build/unsigned)
                                        bool control_sfp = true) const override;

  /**
   * @brief      Apply delay to endpoint
   */
  void apply_endpoint_delay(uint16_t address,      // NOLINT(build/unsigned)
                            uint8_t cycle_delay, // NOLINT(build/unsigned)
                            uint16_t phase_delay) const override;  // NOLINT(build/unsigned)

  /**
   * @brief     Configure fake trigger generator
   */
  void enable_periodic_fl_cmd(uint32_t channel, double rate, bool poisson = false) const override; // NOLINT(build/unsigned)

  /**
   * @brief     Configure fake trigger generator
   */
  void enable_periodic_fl_cmd(uint32_t command, uint32_t channel, double rate, bool poisson = false) const override; // NOLINT(build/unsigned)

  /**
   * @brief      Get master node pointer
   */
  const MasterNodeInterface* get_master_node_plain() const override { return dynamic_cast<const MasterNodeInterface*>(&uhal::Node::getNode("master")); }

  /**
   * @brief      Read master firmware version.
   *
   * @return     { description_of_the_return_value }
   */
  uint32_t read_firmware_version() const override; // NOLINT(build/unsigned)

  /**
   * @brief      Validate master firmware version.
   *
   */
  void validate_firmware_version() const override;

  /**
   * @brief    Give info to collector.
   */
  void get_info(timingfirmwareinfo::TimingDeviceInfo& mon_data) const override;

  /**
   * @brief    Give info to collector.
   */
  void get_info(timingfirmwareinfo::MasterMonitorData& mon_data) const override
  {
    get_master_node_plain()->get_info(mon_data);
  }
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_MASTERDESIGN_HPP_
