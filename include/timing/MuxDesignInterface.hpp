/**
 * @file MuxDesignInterface.hpp
 *
 * MuxDesignInterface is a class providing an interface
 * to firmware design with firmware mux.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_MUXDESIGNINTERFACE_HPP_
#define TIMING_INCLUDE_TIMING_MUXDESIGNINTERFACE_HPP_

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
class MuxDesignInterface : virtual public TopDesignInterface
{

public:
  explicit MuxDesignInterface(const uhal::Node& node) 
    : TopDesignInterface(node) {}
  virtual ~MuxDesignInterface() {}

  /**
   * @brief      Read cdr mux
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint8_t read_active_mux() const = 0; // NOLINT(build/unsigned)

  /**
   * @brief      Switch cdr mux
   *
   * @return     { description_of_the_return_value }
   */
  virtual void switch_mux(uint8_t mux_channel) const = 0; // NOLINT(build/unsigned)

};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_MUXDESIGNINTERFACE_HPP_