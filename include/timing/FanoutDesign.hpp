/**
 * @file FanoutDesign.hpp
 *
 * FanoutDesign is a class providing an interface
 * to the fanout firmware design.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_FANOUTDESIGN_HPP_
#define TIMING_INCLUDE_TIMING_FANOUTDESIGN_HPP_

// PDT Headers
#include "timing/TopDesign.hpp"
#include "timing/EndpointDesignInterface.hpp"
#include "timing/CDRMuxDesignInterface.hpp"

#include "TimingIssues.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <sstream>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for timing fanout designs.
 */
class FanoutDesign : public TopDesign, public EndpointDesignInterface, public CDRMuxDesignInterface
{
  UHAL_DERIVEDNODE(FanoutDesign)
public:
  explicit FanoutDesign(const uhal::Node& node);
  virtual ~FanoutDesign();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  // /**
  //  * @brief    Give info to collector.
  //  */  
  // void get_info(opmonlib::InfoCollector& ci, int level) const override;

  /**
   * @brief      Prepare the timing fanout for data taking.
   *
   */
  void configure() const override;

  /**
   * @brief      Validate endpoint firmware version.
   *
   */
  void validate_firmware_version() const override { /*firmware version in fanout design right now*/}

  uint32_t read_firmware_version() const override { /*firmware version in fanout design right now*/}
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_FANOUTDESIGN_HPP_
