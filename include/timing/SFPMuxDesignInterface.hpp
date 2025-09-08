/**
 * @file SFPMuxDesignInterface.hpp
 *
 * SFPMuxDesignInterface is a base class providing an interface
 * for top level firmware designs on boards with physical muxes.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_SFPMUXDESIGNINTERFACE_HPP_
#define TIMING_INCLUDE_TIMING_SFPMUXDESIGNINTERFACE_HPP_

// PDT Headers
#include "timing/MuxDesignInterface.hpp"
#include "timing/SFPMuxIONode.hpp"
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
class SFPMuxDesignInterface : virtual public MuxDesignInterface
{

public:
  explicit SFPMuxDesignInterface(const uhal::Node& node)
  : TopDesignInterface(node)
  , MuxDesignInterface(node) {}
  virtual ~SFPMuxDesignInterface() {}

  /**
   * @brief     Switch the SFP mux channel
   */
  void switch_mux(uint8_t mux_channel, bool resync_cdr=false) const override // NOLINT(build/unsigned)
  {
    TopDesignInterface::get_io_node<timing::SFPMuxIONode>()->switch_sfp_mux_channel(mux_channel);

    if (resync_cdr)
    {
      resync_active_cdr();
    }
  }

  /**
   * @brief     Read the active SFP mux channel
   */
  uint8_t read_active_mux() const override // NOLINT(build/unsigned)
  {
    return TopDesignInterface::get_io_node<timing::SFPMuxIONode>()->read_active_sfp_mux_channel();
  }

  /**
   * @brief     Scan SFP for alive timing transmitters
   */
  //virtual std::vector<uint32_t> scan_sfp_mux() const = 0;// NOLINT(build/unsigned) // TODO should be SFP specific name?
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_SFPMUXDESIGNINTERFACE_HPP_
