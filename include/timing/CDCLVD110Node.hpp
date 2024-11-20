/**
 * @file CDCLVD110Node.hpp
 *
 * CDCLVD110Node is a class providing an interface
 * to the physical CDCLVD110 IC.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_CDCLVD110NODE_HPP_
#define TIMING_INCLUDE_TIMING_CDCLVD110NODE_HPP_

#include "timing/ClockGeneratorInterface.hpp"

#include "timing/timinghardwareinfo/Structs.hpp"
#include "timing/timinghardwareinfo/Nljs.hpp"

#include "ers/Issue.hpp"

#include <string>

namespace dunedaq {
namespace timing {

/**
 * @class      CDCLVD110Node
 *
 * @brief      uhal::Node implementing CDCLVD110 interface
 */
class CDCLVD110Node
  : public ClockGeneratorInterface
{
  UHAL_DERIVEDNODE(CDCLVD110Node)
public:
  explicit CDCLVD110Node(const uhal::Node& node);
  virtual ~CDCLVD110Node();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief     Get status string, optionally print.
   */
  void get_info(timinghardwareinfo::TimingPLLMonitorData& mon_data) const override;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_CDCLVD110NODE_HPP_