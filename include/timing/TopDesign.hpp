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
template<class IO>
class TopDesign : virtual public TopDesignInterface
{
public:
  explicit TopDesign(const uhal::Node& node)
    : TopDesignInterface(node) {}
  virtual ~TopDesign() {}

  /**
   * @brief      Return the timing IO node.
   *
   * @return     { description_of_the_return_value }
   */
  virtual const IO& get_io_node() const
  {
    return uhal::Node::getNode<IO>("io");
  }
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_TOPDESIGN_HPP_