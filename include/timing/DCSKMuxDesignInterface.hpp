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

#ifndef TIMING_INCLUDE_TIMING_DCSKMUXDESIGNINTERFACE_HPP_
#define TIMING_INCLUDE_TIMING_DCSKMUXDESIGNINTERFACE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/MuxDesignInterface.hpp"
#include "timing/DCSKMuxNode.hpp"

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
class DCSKMuxDesignInterface : virtual public MuxDesignInterface
{

public:
  explicit DCSKMuxDesignInterface(const uhal::Node& node) 
    : MuxDesignInterface(node)
    , TopDesignInterface(node) {}
  virtual ~DCSKMuxDesignInterface() {}

  /**
   * @brief      Read cdr mux
   *
   * @return     { description_of_the_return_value }
   */
  uint8_t read_active_mux() const override // NOLINT(build/unsigned)
  {
    return getNode<DCSKMuxNode>("dcsk_mux").read_active_mux();
  }

  /**
   * @brief      Switch cdr mux
   *
   * @return     { description_of_the_return_value }
   */
  void switch_mux(uint8_t mux_channel, bool resync_cdr=false) const override // NOLINT(build/unsigned)
  {
    // TODO add mux channel validity check
    getNode<DCSKMuxNode>("dcsk_mux").switch_mux(mux_channel);
  }

  /**
   * @brief      Resync active cdr
   *
   * @return     { description_of_the_return_value }
   */
  // TODO move away from here
  void resync_active_cdr() const override // NOLINT(build/unsigned) 
  {
    //auto active_mux = read_active_mux();
    //std::string cdr_path("cdr"+std::to_string(active_mux));
    //getNode<UpstreamCDRNode>(cdr_path).resync();
  }

};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_DCSKMUXDESIGNINTERFACE_HPP_