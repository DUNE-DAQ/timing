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
#include "timing/TimestampGeneratorNode.hpp"
#include "timing/TimingNode.hpp"
#include "timing/timingfirmwareinfo/InfoStructs.hpp"
#include "timing/timingfirmwareinfo/InfoNljs.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"
#include <nlohmann/json.hpp>

#include <string>

namespace dunedaq {
namespace timing {

struct FakeTriggerConfig
{
  double requested_rate;
  uint32_t divisor;  // NOLINT(build/unsigned)
  uint32_t prescale; // NOLINT(build/unsigned)
  double actual_rate;

  explicit FakeTriggerConfig(double rate)
    : requested_rate(rate)
  {
    // Rate =  (50MHz / 2^(d+8)) / p where n in [0,15] and p in [1,256]

    // DIVIDER (int): Frequency divider.

    // The division from 50MHz to the desired rate is done in three steps:
    // a) A pre-division by 256
    // b) Division by a power of two set by n = 2 ^ rate_div_d (ranging from 2^0 -> 2^15)
    // c) 1-in-n prescaling set by n = rate_div_p

    double div = ceil(log(g_dune_sp_clock_in_hz / (requested_rate * 256 * 256)) / log(2));
    if (div < 0) {
      divisor = 0;
    } else if (div > 15) {
      divisor = 15;
    } else {
      divisor = div;
    }

    uint32_t ps = (uint32_t)((g_dune_sp_clock_in_hz / (rate * 256 * (1 << divisor))) + 0.5); // NOLINT(build/unsigned)
    if (ps == 0 || ps > 256) {
      throw BadRequestedFakeTriggerRate(ERS_HERE, rate, ps);
    } else {
      prescale = ps;
    }
    actual_rate = static_cast<double>(g_dune_sp_clock_in_hz) / (256 * prescale * (1 << divisor));
  }

  void print() const
  {
    TLOG() << "Requested rate, actual rate: " << requested_rate << ", " << actual_rate;
    TLOG() << "prescale, divisor: " << prescale << ", " << divisor;
  }
};

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
  void send_fl_cmd(uint32_t command, // NOLINT(build/unsigned)
                   uint32_t channel, // NOLINT(build/unsigned)
                   const TimestampGeneratorNode& timestamp_gen_node) const;

  /**
   * @brief     Configure fake trigger
   */
  void enable_fake_trigger(uint32_t channel,  // NOLINT(build/unsigned)
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

  /**
   * @brief     Fill the fixed length command counters monitoring structure.
   */
  void get_info(timingfirmwareinfo::TimingFLCmdChannelCounters& mon_data) const;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_FLCMDGENERATORNODE_HPP_