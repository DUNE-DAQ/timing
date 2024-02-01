/**
 * @file CDRMuxDesignInterface.hpp
 *
 * CDRMuxDesignInterface is a class providing an interface
 * to firmware design with firmware mux.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_CDRMUXDESIGNINTERFACE_HPP_
#define TIMING_INCLUDE_TIMING_CDRMUXDESIGNINTERFACE_HPP_

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
class CDRMuxDesignInterface : virtual public TopDesignInterface
{

public:
  explicit CDRMuxDesignInterface(const uhal::Node& node) 
    : TopDesignInterface(node) {}
  virtual ~CDRMuxDesignInterface() {}

  /**
   * @brief      Read cdr mux
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint8_t read_active_cdr_mux() const // NOLINT(build/unsigned)
  {
    auto active_sfp_mux_channel = getNode("us_mux.csr.ctrl.src").read();
    getClient().dispatch();
    return active_sfp_mux_channel.value();
  }

  /**
   * @brief      Switch cdr mux
   *
   * @return     { description_of_the_return_value }
   */
  virtual void switch_cdr_mux(uint8_t mux_channel) const // NOLINT(build/unsigned)
  {
    // TODO add mux channel validity check
    getNode("us_mux.csr.ctrl.src").write(mux_channel);
    getClient().dispatch();
  }

};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_CDRMUXDESIGNINTERFACE_HPP_