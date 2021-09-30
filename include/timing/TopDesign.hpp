/**
 * @file TopDesign.hpp
 *
 * TopDesign is a class providing the base interface
 * for timing top design nodes.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_TOPDESIGN_HPP_
#define TIMING_INCLUDE_TIMING_TOPDESIGN_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/EndpointNode.hpp"
#include "timing/TimingNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

#include <nlohmann/json.hpp>

// C++ Headers
#include <chrono>
#include <sstream>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Base class for timing top design nodes.
 */
template<class IO>
class TopDesign : public TimingNode
{
public:
  explicit TopDesign(const uhal::Node& node);
  virtual ~TopDesign();

  /**
   * @brief      Reset timing node.
   */
  virtual void soft_reset() const;

  /**
   * @brief      Reset timing node.
   */
  virtual void reset(const std::string& clock_config_file = "") const;

  /**
   * @brief      Return the timing IO node.
   *
   * @return     { description_of_the_return_value }
   */
  virtual const IO& get_io_node() const;

  /**
   * @brief      Print hardware information
   */
  virtual std::string get_hardware_info(bool print_out = false) const;

  /**
   * @brief      Return the timing endpoint node.
   *
   * @return     { description_of_the_return_value }
   */
  virtual const EndpointNode& get_endpoint_node(uint32_t ept_id) const; // NOLINT(build/unsigned)

  /**
   * @brief      Return the timing endpoint node.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint32_t get_number_of_endpoint_nodes() const; // NOLINT(build/unsigned)

  /**
   * @brief      Read firmware version.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint32_t read_firmware_version() const = 0; // NOLINT(build/unsigned)

  /**
   * @brief      Validate firmware version.
   *
   */
  virtual void validate_firmware_version() const = 0;
  
  /**
   * @brief      Collect operational monitoring information
   */
  template<class T>
  void get_info(T& data) const
  {}
};

} // namespace timing
} // namespace dunedaq

#include "timing/detail/TopDesign.hxx"

#endif // TIMING_INCLUDE_TIMING_TOPDESIGN_HPP_