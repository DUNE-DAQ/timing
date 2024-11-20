/**
 * @file ClockGeneratorInterface.hpp
 *
 * ClockGeneratorInterface is a class providing an interface
 * to the physical CDCLVD110 IC.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_CLOCKGENERATORINTERFACE_HPP_
#define TIMING_INCLUDE_TIMING_CLOCKGENERATORINTERFACE_HPP_

#include "timing/TimingNode.hpp"

#include "timing/timinghardwareinfo/Structs.hpp"
#include "timing/timinghardwareinfo/Nljs.hpp"

#include "ers/Issue.hpp"

#include <string>

namespace dunedaq {
namespace timing {

/**
 * @class      ClockGeneratorInterface
 *
 * @brief      uhal::Node implementing clock gen. interface
 */
class ClockGeneratorInterface : public TimingNode
{
public:
  explicit ClockGeneratorInterface(const uhal::Node& node) : TimingNode(node) {}
  virtual ~ClockGeneratorInterface() {}

  /**
   * @brief     Get status string, optionally print.
   */
  virtual void get_info(timinghardwareinfo::TimingPLLMonitorData& mon_data) const = 0;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_CLOCKGENERATORINTERFACE_HPP_