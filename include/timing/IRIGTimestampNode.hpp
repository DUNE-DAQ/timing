/**
 * @file IRIGTimestampNode.hpp
 *
 * IRIGTimestampNode is a class providing an interface
 * to timestamp generator firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_IRIGTIMESTAMPNODE_HPP_
#define TIMING_INCLUDE_TIMING_IRIGTIMESTAMPNODE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/TimingNode.hpp"
#include "timing/definitions.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for timestamp generator node.
 */
class IRIGTimestampNode : public TimingNode
{
  UHAL_DERIVEDNODE(IRIGTimestampNode)
public:
  explicit IRIGTimestampNode(const uhal::Node& node);
  virtual ~IRIGTimestampNode();

  /**
   * @brief     Print the status of the timing node.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief      Read the current timestamp words.
   *
   * @return     { description_of_the_return_value }
   */
  uhal::ValVector<uint32_t> read_raw_timestamp(bool dispatch = true) const; // NOLINT(build/unsigned)

  /**
   * @brief      Read the current timestamp words.
   *
   * @return     { description_of_the_return_value }
   */
  uint64_t read_timestamp() const; // NOLINT(build/unsigned)

  /**
   * @brief      Set epoch: UNIX/custom
   */
  void set_ts_epoch(TimestampEpoch ts_epoch) const; // NOLINT(build/unsigned)

  /**
   * @brief      Set custom epoch value
   */
  void set_ts_epoch_value(uint64_t epoch_to_2000_seconds_tai, uint8_t epoch_to_2000_leap_seconds) const; // NOLINT(build/unsigned)

  /**
   * @brief      Set custom epoch date
   */
  //void set_ts_epoch_date(uint64_t epoch_to_2000_seconds_tai, uint8_t epoch_to_2000_leap_seconds) const; // NOLINT(build/unsigned)

  /**
   * @brief      Set timebase: TAI/UTC
   */
  void set_ts_timebase(TimestampTimebase ts_timebase) const; // NOLINT(build/unsigned)

  /**
   * @brief      Set seconds offset
   */
  void set_ts_seconds_offset(int8_t seconds_offset) const; // NOLINT(build/signed)

  /**
   * @brief      Set ticks offset
   */
  void set_ts_ticks_offset(int16_t ticks_offset) const; // NOLINT(build/signed)

  /**
   * @brief      Read the current pps ctr word.
   *
   * @return     { description_of_the_return_value }
   */
  uint32_t read_pps_counter() const; // NOLINT(build/unsigned)

  /**
   * @brief      Read the current seconds since epoch words.
   *
   * @return     { description_of_the_return_value }
   */
  uint64_t read_seconds_since_epoch() const; // NOLINT(build/unsigned)
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_IRIGTIMESTAMPNODE_HPP_