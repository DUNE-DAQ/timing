/**
 * @file FanoutIONode.hpp
 *
 * FanoutIONode is a base class providing an interface
 * to fanout IO firmware blocks.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_FANOUTIONODE_HPP_
#define TIMING_INCLUDE_TIMING_FANOUTIONODE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/IONode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

/**
 * @brief      Base class for the fanout boards.
 */
class FanoutIONode : public IONode
{

public:
  FanoutIONode(const uhal::Node& node,
               std::string uid_i2c_bus,
               std::string pll_i2c_bus,
               std::string pll_i2c_device,
               std::vector<std::string> clock_names,
               std::vector<std::string> sfp_i2c_buses);
  virtual ~FanoutIONode();

  /**
   * @brief     Switch the SFP mux channel
   */
  virtual void switch_downstream_mux_channel(uint32_t mux_channel) const = 0; // NOLINT(build/unsigned)

  /**
   * @brief     Read the active SFP mux channel
   */
  virtual uint32_t read_active_downstream_mux_channel() const = 0; // NOLINT(build/unsigned)
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_FANOUTIONODE_HPP_