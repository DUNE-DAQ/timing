/**
 * @file MasterGlobalNode.hpp
 *
 * MasterGlobalNode is a class providing an interface
 * to the master global firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_MASTERGLOBALNODE_HPP_
#define TIMING_INCLUDE_TIMING_MASTERGLOBALNODE_HPP_

// PDT Headers
#include "timing/TimingNode.hpp"
#include "timing/toolbox.hpp"
// uHal Headers
#include "uhal/DerivedNode.hpp"

#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for master global node.
 */
class MasterGlobalNode : public TimingNode
{
  UHAL_DERIVEDNODE(MasterGlobalNode)
public:
  explicit MasterGlobalNode(const uhal::Node& node);
  virtual ~MasterGlobalNode();
  
  /**
   * @brief     Enable the upstream endpoint.
   */
  void enable_upstream_endpoint(uint32_t timeout = 500) const; // NOLINT(build/unsigned)

  /**
   * @brief     Read the upstream endpoint ready reg.
   */
  bool read_upstream_endpoint_ready() const; // NOLINT(build/unsigned)

  /**
   * @brief     Enable the upstream endpoint.
   */
  void reset_command_counters(uint32_t timeout = 500) const; // NOLINT(build/unsigned)

  /**
   * @brief     Enable the upstream endpoint.
   */
  bool read_counters_ready() const; // NOLINT(build/unsigned)

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_MASTERGLOBALNODE_HPP_