/**
 * @file MuxDesignInterface.hpp
 *
 * MuxDesignInterface is a base class providing an interface
 * to for top level firmware designs on boards with muxes.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_MUXDESIGNINTERFACE_HPP_
#define TIMING_INCLUDE_TIMING_MUXDESIGNINTERFACE_HPP_

// PDT Headers
#include "timing/TopDesignInterface.hpp"
#include "timing/FanoutIONode.hpp"
#include "TimingIssues.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <sstream>
#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for timing fanout designs.
 */
class MuxDesignInterface : virtual public TopDesignInterface
{

public:
  explicit MuxDesignInterface(const uhal::Node& node)
  : TopDesignInterface(node) {}
  virtual ~MuxDesignInterface() {}

  /**
   * @brief     Switch the SFP mux channel
   */
  virtual void switch_downstream_mux_channel(uint32_t sfp_id, bool /*wait_for_rtt_ept_lock*/) const = 0;// NOLINT(build/unsigned)

  /**
   * @brief     Read the active SFP mux channel
   */
  virtual uint32_t read_active_downstream_mux_channel() const // NOLINT(build/unsigned)
  {
    return TopDesignInterface::get_io_node<timing::FanoutIONode>()->read_active_downstream_mux_channel();
  }

  /**
   * @brief     Scan SFP for alive timing transmitters
   */
  virtual std::vector<uint32_t> scan_sfp_mux() const = 0;// NOLINT(build/unsigned)
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_MUXDESIGNINTERFACE_HPP_
