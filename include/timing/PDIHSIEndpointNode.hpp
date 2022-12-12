/**
 * @file PDIHSIEndpointNode.hpp
 *
 * PDIHSIEndpointNode is a class providing an interface
 * to the endpoint wrapper firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_PDIHSIENDPOINTNODE_HPP_
#define TIMING_INCLUDE_TIMING_PDIHSIENDPOINTNODE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/EndpointNodeInterface.hpp"
#include "timing/FrequencyCounterNode.hpp"
#include "timing/HSINode.hpp"

#include "timing/timingendpointinfo/InfoNljs.hpp"
#include "timing/timingendpointinfo/InfoStructs.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

#include <nlohmann/json.hpp>

// C++ Headers
#include <chrono>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Base class for timing IO nodes.
 */
class PDIHSIEndpointNode : public EndpointNodeInterface
{
  UHAL_DERIVEDNODE(PDIHSIEndpointNode)
public:
  explicit PDIHSIEndpointNode(const uhal::Node& node);
  virtual ~PDIHSIEndpointNode();

  /**
   * @brief     Print the status of the timing node.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief      Enable the endpoint
   *
   * @return     { description_of_the_return_value }
   */
  void enable(uint32_t address = 0, uint32_t partition = 0) const override; // NOLINT(build/unsigned)

  /**
   * @brief      Disable the endpoint
   *
   * @return     { description_of_the_return_value }
   */
  void disable() const override;

  /**
   * @brief      Reset the endpoint
   *
   * @return     { description_of_the_return_value }
   */
  void reset(uint32_t address = 0, uint32_t partition = 0) const override; // NOLINT(build/unsigned)

  /**
   * @brief      Get wrapped hsi node
   *
   * @return     { description_of_the_return_value }
   */
  const HSINode& get_hsi_node() const { return uhal::Node::getNode<HSINode>("hsi"); }

  /**
   * @brief     Collect monitoring information for timing endpoint
   *
   */
  virtual void get_info(timingendpointinfo::TimingEndpointInfo& mon_data) const;

  /**
   * @brief    Give info to collector.
   */
  void get_info(opmonlib::InfoCollector& ci, int level) const override;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_PDIHSIENDPOINTNODE_HPP_