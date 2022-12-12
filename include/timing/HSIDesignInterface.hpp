/**
 * @file HSIDesignInterface.hpp
 *
 * HSIDesignInterface is a class providing an interface
 * to the endpoint firmware design.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_HSIDESIGNINTERFACE_HPP_
#define TIMING_INCLUDE_TIMING_HSIDESIGNINTERFACE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/EndpointDesignInterface.hpp"
#include "timing/HSINode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <sstream>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Base class for timing hsi design nodes.
 */
class HSIDesignInterface : virtual public EndpointDesignInterface
{

public:
  explicit HSIDesignInterface(const uhal::Node& node)
    : EndpointDesignInterface(node)
  {
  }
  virtual ~HSIDesignInterface() {}

  /**
   * @brief      Get the HSI node.
   *
   * @return     { description_of_the_return_value }
   */
  virtual const HSINode& get_hsi_node() const
  {

    auto top_level_hsi_nodes = getNodes("hsi");
    if (top_level_hsi_nodes.size() > 0) {
      return uhal::Node::getNode<HSINode>("hsi");
    } else {
      return uhal::Node::getNode<HSINode>("endpoint0.hsi");
    }
  }

  /** @brief      Configure the HSI node.
   *
   * @return     { description_of_the_return_value }
   */
  virtual void configure_hsi(uint32_t src,      // NOLINT(build/unsigned)
                             uint32_t re_mask,  // NOLINT(build/unsigned)
                             uint32_t fe_mask,  // NOLINT(build/unsigned)
                             uint32_t inv_mask, // NOLINT(build/unsigned)
                             double rate,
                             bool dispatch = true) const
  {
    uint32_t firmware_frequency = get_io_node_plain()->read_firmware_frequency(); // NOLINT(build/unsigned)
    get_hsi_node().configure_hsi(src, re_mask, fe_mask, inv_mask, rate, firmware_frequency, dispatch);
  }
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_HSIDESIGNINTERFACE_HPP_