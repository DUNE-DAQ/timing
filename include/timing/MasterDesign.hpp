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
template<class IO, class MST>
class MasterDesign : public TopDesign<IO>, virtual public MasterDesignInterface
{

public:
  explicit MasterDesign(const uhal::Node& node);
  virtual ~MasterDesign();

  using MasterDesignInterface::get_master_node;

  /**
   * @brief      Get the timing master node.
   *
   * @return     { description_of_the_return_value }
   */
  virtual const MST& get_master_node() const;

  
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
  void sync_timestamp() const override;
  
  /**
   * @brief      Measure the endpoint round trip time.
   *
   * @return     { description_of_the_return_value }
   */
  uint32_t measure_endpoint_rtt(uint32_t address, bool control_sfp = true) const override; // NOLINT(build/unsigned)

  /**
   * @brief      Apply delay to endpoint
   */
  void apply_endpoint_delay(uint32_t address,      // NOLINT(build/unsigned)
                            uint32_t coarse_delay, // NOLINT(build/unsigned)
                            uint32_t fine_delay,   // NOLINT(build/unsigned)
                            uint32_t phase_delay,  // NOLINT(build/unsigned)
                            bool measure_rtt = false,
                            bool control_sfp = true) const override;
  /**
   * @brief     Send a fixed length command
   */
  void send_fl_cmd(uint32_t command,                       // NOLINT(build/unsigned)
                   uint32_t channel,                       // NOLINT(build/unsigned)
                   uint32_t number_of_commands = 1) const override; // NOLINT(build/unsigned)

  /**
   * @brief     Configure fake trigger generator
   */
  void enable_fake_trigger(uint32_t channel, double rate, bool poisson = false) const override; // NOLINT(build/unsigned)

  /**
   * @brief      Read master firmware version.
   *
   * @return     { description_of_the_return_value }
   */
  uint32_t read_firmware_version() const override;

  /**
   * @brief      Validate master firmware version.
   *
   */
  void validate_firmware_version() const override;

  /**
   * @brief    Give info to collector.
   */  
  void get_info(opmonlib::InfoCollector& ci, int level) const override;

  using TopDesign<IO>::get_io_node;
};

} // namespace timing
} // namespace dunedaq

#include "timing/detail/MasterDesign.hxx"

#endif // TIMING_INCLUDE_TIMING_MASTERDESIGN_HPP_