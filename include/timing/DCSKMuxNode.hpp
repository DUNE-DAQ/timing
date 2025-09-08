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

#ifndef TIMING_INCLUDE_TIMING_DCSKMUXNODE_HPP_
#define TIMING_INCLUDE_TIMING_DCSKMUXNODE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/MuxNodeInterface.hpp"

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
class DCSKMuxNode : public MuxNodeInterface
{
    UHAL_DERIVEDNODE(DCSKMuxNode)
public:
  explicit DCSKMuxNode(const uhal::Node& node);
  virtual ~DCSKMuxNode();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief      Read cdr mux
   *
   * @return     { description_of_the_return_value }
   */
  uint8_t read_active_mux() const; // NOLINT(build/unsigned)


  /**
   * @brief      Switch cdr mux
   *
   * @return     { description_of_the_return_value }
   */
  void switch_mux(uint8_t mux_channel) const; // NOLINT(build/unsigned)

  void set_mux_sel_source(uint8_t source) const; // NOLINT(build/unsigned)

};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_DCSKMUXNODE_HPP_