/**
 * @file TopDesignInterface.hpp
 *
 * TopDesignInterface is a class providing the base interface
 * for timing top design nodes.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_TOPDESIGNINTERFACE_HPP_
#define TIMING_INCLUDE_TIMING_TOPDESIGNINTERFACE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/TimingNode.hpp"
#include "timing/IONode.hpp"

#include "timing/timingfirmwareinfo/Structs.hpp"
#include "timing/timingfirmwareinfo/Nljs.hpp"

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
class TopDesignInterface : public TimingNode
{
public:
  explicit TopDesignInterface(const uhal::Node& node) 
    : TimingNode(node) {}
  virtual ~TopDesignInterface() {}

  /**
   * @brief      Get io node pointer
   */
  virtual const IONode* get_io_node_plain() const = 0;
  

  template<class IO>
  const IO* get_io_node() const
  { 
    return dynamic_cast<const IO*>(get_io_node_plain()); 
  }

  /**
   * @brief      Reset timing node.
   */
  virtual void soft_reset_io() const = 0;

  /**
   * @brief      Reset timing node.
   */
  virtual void reset_io(const std::string& clock_config_file) const = 0;

  /**
   * @brief      Reset timing node.
   */
  virtual void reset_io(const ClockSource& clock_source) const = 0;

  /**
   * @brief      Prepare the timing device for data taking.
   *
   */
  virtual void configure() const = 0;

  /**
   * @brief      Print hardware information
   */
  virtual std::string get_hardware_info(bool print_out = false) const = 0;

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
   * @brief    Give info to collector.
   */
  virtual void get_info(timingfirmwareinfo::TimingDeviceInfo& mon_data) const = 0;

};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_TOPDESIGNINTERFACE_HPP_