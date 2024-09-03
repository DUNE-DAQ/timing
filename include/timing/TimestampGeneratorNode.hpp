/**
 * @file TimestampGeneratorNode.hpp
 *
 * TimestampGeneratorNode is a class providing an interface
 * to timestamp generator firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_TIMESTAMPGENERATORNODE_HPP_
#define TIMING_INCLUDE_TIMING_TIMESTAMPGENERATORNODE_HPP_

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
class TimestampGeneratorNode : public TimingNode
{
  UHAL_DERIVEDNODE(TimestampGeneratorNode)
public:
  explicit TimestampGeneratorNode(const uhal::Node& node);
  virtual ~TimestampGeneratorNode();

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
   * @brief      Read the starting timestamp words.
   *
   * @return     { description_of_the_return_value }
   */
  uint64_t read_start_timestamp() const; // NOLINT(build/unsigned)

  /**
   * @brief      Read the software init timestamp words.
   *
   * @return     { description_of_the_return_value }
   */
  uint64_t read_sw_init_timestamp() const; // NOLINT(build/unsigned)

  /**
   * @brief      Initialise timestamp.
   */
  void set_timestamp(TimestampSource source) const; // NOLINT(build/unsigned)
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_TIMESTAMPGENERATORNODE_HPP_