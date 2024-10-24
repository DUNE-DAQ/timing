/**
 * @file FLCmdGeneratorNode.hpp
 *
 * FLCmdGeneratorNode is a class providing an interface
 * to the fixed length command generator firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_FLCMDGENERATORNODE_HPP_
#define TIMING_INCLUDE_TIMING_FLCMDGENERATORNODE_HPP_

// PDT Headers
#include "timing/definitions.hpp"
#include "timing/TimestampGeneratorNode.hpp"
#include "timing/TimingNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"
#include <nlohmann/json.hpp>

#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for master global node.
 */
class FLCmdGeneratorNode : public TimingNode
{
  UHAL_DERIVEDNODE(FLCmdGeneratorNode)
public:
  explicit FLCmdGeneratorNode(const uhal::Node& node);
  virtual ~FLCmdGeneratorNode();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief     Send a fixed length command
   */
  virtual void send_fl_cmd(uint32_t command,        // NOLINT(build/unsigned)
                           uint32_t channel) const; // NOLINT(build/unsigned)
  /**
   * @brief     Configure fake trigger
   */
  void enable_periodic_fl_cmd(uint32_t channel,  // NOLINT(build/unsigned)
                              double rate,  // NOLINT(build/unsigned)
                              bool poisson,
                              uint32_t clock_frequency_hz) const; // NOLINT(build/unsigned)

  /**
   * @brief     Configure fake trigger
   */
  void enable_periodic_fl_cmd(uint32_t command,  // NOLINT(build/unsigned)
                              uint32_t channel,  // NOLINT(build/unsigned)
                              double rate, // NOLINT(build/unsigned)
                              bool poisson,
                              uint32_t clock_frequency_hz) const; // NOLINT(build/unsigned)

  /**
   * @brief     Configure fake trigger
   */
  void enable_periodic_fl_cmd(uint32_t channel,  // NOLINT(build/unsigned)
                           uint32_t divisor,  // NOLINT(build/unsigned)
                           uint32_t prescale, // NOLINT(build/unsigned)
                           bool poisson) const;

  /**
   * @brief     Configure fake trigger
   */
  void enable_periodic_fl_cmd(uint32_t command,  // NOLINT(build/unsigned)
                           uint32_t channel,  // NOLINT(build/unsigned)
                           uint32_t divisor,  // NOLINT(build/unsigned)
                           uint32_t prescale, // NOLINT(build/unsigned)
                           bool poisson) const;
  
  /**
   * @brief     Clear fake trigger configuration
   */
  void disable_fake_trigger(uint32_t channel) const; // NOLINT(build/unsigned)

  /**
   * @brief     Get command counters status string
   */
  std::string get_cmd_counters_table(bool print_out = false) const;

  static void parse_periodic_fl_cmd_rate(double requested_rate, uint32_t clock_frequency_hz, double& actual_rate, uint32_t& divisor, uint32_t& prescale);
private:
  void validate_command(uint32_t command) const;
  void validate_channel(uint32_t channel) const;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_FLCMDGENERATORNODE_HPP_
