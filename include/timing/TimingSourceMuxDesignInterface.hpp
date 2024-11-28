/**
 * @file TimingSourceMuxDesignInterface.hpp
 *
 * TimingSourceMuxDesignInterface is a class providing an interface
 * to firmware design with a timing source mux.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_TIMINGSOURCEMUXDESIGNINTERFACE_HPP_
#define TIMING_INCLUDE_TIMING_TIMINGSOURCEMUXDESIGNINTERFACE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/TopDesignInterface.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <sstream>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Base class for timing endpoint design nodes.
 */
class TimingSourceMuxDesignInterface : virtual public TopDesignInterface
{

public:
  explicit TimingSourceMuxDesignInterface(const uhal::Node& node) 
    : TopDesignInterface(node) {}
  virtual ~TimingSourceMuxDesignInterface() {}

  /**
   * @brief      Read timing source mux
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint8_t read_active_timing_source_mux() const // NOLINT(build/unsigned)
  {
    auto active_sfp_mux_channel = getNode("timing_source_mux.csr.ctrl.src").read();
    getClient().dispatch();
    return active_sfp_mux_channel.value();
  }

  /**
   * @brief      Switch timing source mux
   *
   * @return     { description_of_the_return_value }
   */
  virtual void switch_timing_source_mux(uint8_t mux_channel) const // NOLINT(build/unsigned)
  {
    // TODO add mux channel validity check
    getNode("timing_source_mux.csr.ctrl.src").write(mux_channel);
    getClient().dispatch();
  }

  /**
   * @brief      Switch timing source
   *
   */
  virtual void switch_timing_source(ClockSource clock_source) const = 0;// NOLINT(build/unsigned)

};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_TIMINGSOURCEMUXDESIGNINTERFACE_HPP_