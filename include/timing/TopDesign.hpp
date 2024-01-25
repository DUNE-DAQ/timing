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
#include "timing/TopDesignInterface.hpp"

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
 * @brief      Base class for timing top design nodes with IO class.
 */
class TopDesign : virtual public TopDesignInterface
{
public:
  explicit TopDesign(const uhal::Node& node)
    : TopDesignInterface(node) {}
  virtual ~TopDesign() {}

  /**
   * @brief      Get io node pointer
   */
  const IONode* get_io_node_plain() const override
  { 
    return dynamic_cast<const IONode*>(&uhal::Node::getNode("io")); 
  }

  /**
   * @brief      Reset timing node.
   */
  void soft_reset_io() const override
  {
    get_io_node_plain()->soft_reset();
  }

  /**
   * @brief      Reset timing node.
   */
  void reset_io(const std::string& clock_config_file) const override
  {
    get_io_node_plain()->reset(clock_config_file);
  }

  /**
   * @brief      Reset timing node.
   */
  void reset_io(const ClockSource& clock_source) const override
  {
    get_io_node_plain()->reset(clock_source);
  }

  /**
   * @brief      Print hardware information
   */
  std::string get_hardware_info(bool print_out = false) const override
  {
    auto info = get_io_node_plain()->get_hardware_info();
    if (print_out)
      TLOG() << info;
    return info;
  }
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_TOPDESIGN_HPP_