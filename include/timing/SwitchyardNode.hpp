/**
 * @file SwitchyardNode.hpp
 *
 * SwitchyardNode is a class providing an interface
 * to spill interface firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_SWITCHYARDNODE_HPP_
#define TIMING_INCLUDE_TIMING_SWITCHYARDNODE_HPP_

// PDT Headers
#include "timing/TimingNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for master global node.
 */
class SwitchyardNode : public TimingNode
{
  UHAL_DERIVEDNODE(SwitchyardNode)
public:
  explicit SwitchyardNode(const uhal::Node& node);
  virtual ~SwitchyardNode();

  /**
   * @brief     Print the status of the timing node.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief     Configure master source
   */
  void configure_master_source(uint8_t master_source, bool dispatch=true) const;

  /**
   * @brief     Configure endpoint source
   */
  void configure_endpoint_source(uint8_t endpoint_source, bool dispatch=true) const;

};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_SWITCHYARDNODE_HPP_