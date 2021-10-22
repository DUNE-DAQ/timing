/**
 * @file ChronosDesign.hpp
 *
 * ChronosDesign is a base class providing an interface
 * to top level HSI endpoint firmware designs.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_CHRONOSDESIGN_HPP_
#define TIMING_INCLUDE_TIMING_CHRONOSDESIGN_HPP_

// Timing Headers
#include "timing/HSINode.hpp"
#include "timing/EndpointDesign.hpp"

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
class ChronosDesign : public EndpointDesign<IO>
{

public:
  explicit ChronosDesign(const uhal::Node& node);
  virtual ~ChronosDesign();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;


  /**
   * @brief      Get the HSI node.
   *
   * @return     { description_of_the_return_value }
   */
  virtual const HSINode& get_hsi_node() const { return uhal::Node::getNode<HSINode>("endpoint0"); }

  /**
   * @brief      Read endpoint firmware version.
   *
   * @return     { description_of_the_return_value }
   */
  uint32_t read_firmware_version() const override {return 0;} // current chronos firmware does not store firmware version

  /**
   * @brief      Validate endpoint firmware version.
   *
   */
  void validate_firmware_version() const override {} // current chronos firmware does not store firmware version

  /** @brief      Configure the HSI node.
   *
   * @return     { description_of_the_return_value }
   */
  virtual void configure_hsi(uint32_t src,      // NOLINT(build/unsigned)
                             uint32_t re_mask,  // NOLINT(build/unsigned)
                             uint32_t fe_mask,  // NOLINT(build/unsigned)
                             uint32_t inv_mask, // NOLINT(build/unsigned)
                             double rate,
                             bool dispatch = true) const;

  template<class T>
  void get_info(T& data) const;
  
  // In leiu of UHAL_DERIVEDNODE
protected:
  virtual uhal::Node* clone() const;
  //
};

} // namespace timing
} // namespace dunedaq

#include "timing/detail/ChronosDesign.hxx"

#endif // TIMING_INCLUDE_TIMING_CHRONOSDESIGN_HPP_