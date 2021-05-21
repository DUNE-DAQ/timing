/**
 * @file FrequencyCounterNode.hpp
 *
 * FrequencyCounterNode is a class providing an interface
 * to the frequency counter firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_FREQUENCYCOUNTERNODE_HPP_
#define TIMING_INCLUDE_TIMING_FREQUENCYCOUNTERNODE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/TimingNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <string>
#include <thread>
#include <vector>

namespace dunedaq {
namespace timing {

/**
 * @brief      Base class for timing IO nodes.
 */
class FrequencyCounterNode : public TimingNode
{
  UHAL_DERIVEDNODE(FrequencyCounterNode)
public:
  explicit FrequencyCounterNode(const uhal::Node& node);
  virtual ~FrequencyCounterNode();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief     Measure clock frequencies.
   *
   * @return     { description_of_the_return_value }
   */
  std::vector<double> measure_frequencies(uint8_t number_of_clocks) const; // NOLINT(build/unsigned)
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_FREQUENCYCOUNTERNODE_HPP_