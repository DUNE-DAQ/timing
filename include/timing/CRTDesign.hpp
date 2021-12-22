/**
 * @file CRTDesign.hpp
 *
 * CRTDesign is a class providing an interface
 * to the top level CRT firmware design.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_CRTDESIGN_HPP_
#define TIMING_INCLUDE_TIMING_CRTDESIGN_HPP_

// Timing Headers
#include "timing/CRTNode.hpp"
#include "timing/TopDesign.hpp"
#include "timing/CRTDesignInterface.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <sstream>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for timing master with integrated HSI designs.
 */
template<class IO>
class CRTDesign : public TopDesign<IO>, public CRTDesignInterface
{

public:
  explicit CRTDesign(const uhal::Node& node);
  virtual ~CRTDesign();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief      Prepare the timing endpoint for data taking.
   *
   */
  void configure() const override;

  /**
   * @brief      Read endpoint firmware version.
   *
   * @return     { description_of_the_return_value }
   */
  uint32_t read_firmware_version() const override { // NOLINT(build/unsigned)
    // current crt firmware does not store firmware version
    return 0; 
  } 

  /**
   * @brief      Validate endpoint firmware version.
   *
   */
  void validate_firmware_version() const override {} // current crt firmware does not store firmware version
  
  // In leiu of UHAL_DERIVEDNODE
protected:
  virtual uhal::Node* clone() const;
  //
};

} // namespace timing
} // namespace dunedaq

#include "timing/detail/CRTDesign.hxx"

#endif // TIMING_INCLUDE_TIMING_CRTDESIGN_HPP_