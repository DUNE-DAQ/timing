/**
 * @file UpstreamCDRNode.hpp
 *
 * UpstreamCDRNode is a class providing an interface
 * to the upstream CDR firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_UPSTREAMCDRNODE_HPP_
#define TIMING_INCLUDE_TIMING_UPSTREAMCDRNODE_HPP_

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
class UpstreamCDRNode : public TimingNode
{
  UHAL_DERIVEDNODE(UpstreamCDRNode)
public:
  explicit UpstreamCDRNode(const uhal::Node& node);
  virtual ~UpstreamCDRNode();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief     Reync CDR.
   */
  void resync() const;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_UPSTREAMCDRNODE_HPP_