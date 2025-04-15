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
#include "timing/MuxDesignInterface.hpp"
#include "timing/UpstreamCDRNode.hpp"

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
class CDRMuxDesignInterface : virtual public MuxDesignInterface
{

public:
  explicit CDRMuxDesignInterface(const uhal::Node& node) 
    : TopDesignInterface(node)
    , MuxDesignInterface(node) {}
  virtual ~CDRMuxDesignInterface() {}

  /**
   * @brief      Read cdr mux
   *
   * @return     { description_of_the_return_value }
   */
  uint8_t read_active_mux() const override // NOLINT(build/unsigned)
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
  void switch_mux(uint8_t mux_channel, bool resync_cdr=false) const override // NOLINT(build/unsigned)
  {
    // TODO add mux channel validity check
    getNode("us_mux.csr.ctrl.src").write(mux_channel);
    getClient().dispatch();

    if (resync_cdr)
    {
      resync_active_cdr();
    }
  }

  /**
   * @brief      Resync active cdr
   *
   * @return     { description_of_the_return_value }
   */
  void resync_active_cdr() const override // NOLINT(build/unsigned)
  {
    auto active_mux = read_active_mux();
    std::string cdr_path("cdr"+std::to_string(active_mux));
    getNode<UpstreamCDRNode>(cdr_path).resync();
  }

};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_CDRMUXDESIGNINTERFACE_HPP_