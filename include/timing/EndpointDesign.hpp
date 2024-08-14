/**
 * @file EndpointDesign.hpp
 *
 * EndpointDesign is a class providing an interface
 * to the endpoint firmware design.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_ENDPOINTDESIGN_HPP_
#define TIMING_INCLUDE_TIMING_ENDPOINTDESIGN_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/FMCIONode.hpp"
#include "timing/TopDesign.hpp"
#include "timing/EndpointDesignInterface.hpp"

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
class EndpointDesign : public TopDesign, public EndpointDesignInterface
{
  UHAL_DERIVEDNODE(EndpointDesign)
public:
  explicit EndpointDesign(const uhal::Node& node);
  virtual ~EndpointDesign();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief      Prepare the timing endpoint for data taking.
   *
   */
  void configure() const override;

  // /**
  //  * @brief    Give info to collector.
  //  */  
  // void get_info(opmonlib::InfoCollector& ci, int level) const override;

  /**
   * @brief      Read endpoint firmware version.
   *
   * @return     { description_of_the_return_value }
   */
  uint32_t read_firmware_version() const override; // NOLINT(build/unsigned)

  /**
   * @brief      Validate endpoint firmware version.
   *
   */
  void validate_firmware_version() const override;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_ENDPOINTDESIGN_HPP_
