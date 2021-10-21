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
  template<class IO>
  const IO* get_io_node() const { return dynamic_cast<const IO*>(&uhal::Node::getNode("io")); }

  /**
   * @brief      Reset timing node.
   */
  virtual void soft_reset_io() const
  {
    get_io_node<IONode>()->soft_reset();
  }

  /**
   * @brief      Reset timing node.
   */
  virtual void reset_io(const std::string& clock_config_file = "") const
  {
    get_io_node<IONode>()->reset(clock_config_file);
  }

  /**
   * @brief      Reset timing node.
   */
  virtual void reset_io(int32_t fanout_mode, // NOLINT(build/unsigned)
                        const std::string& clock_config_file = "") const
  {
    get_io_node<IONode>()->reset(fanout_mode, clock_config_file);
  }

  /**
   * @brief      Prepare the timing device for data taking.
   *
   */
  virtual void configure() const = 0;

  /**
   * @brief      Print hardware information
   */
  virtual std::string get_hardware_info(bool print_out = false) const
  {
    auto info = get_io_node<IONode>()->get_hardware_info();
    if (print_out)
      TLOG() << info;
    return info;
  }

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

};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_TOPDESIGNINTERFACE_HPP_