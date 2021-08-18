/**
 * @file FMCIONode.hpp
 *
 * FMCIONode is a class providing an interface
 * to the FMC IO firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_FMCIONODE_HPP_
#define TIMING_INCLUDE_TIMING_FMCIONODE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/IONode.hpp"
#include "timing/timinghardwareinfo/InfoStructs.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for the timing FMC board.
 */
class FMCIONode : public IONode
{
  UHAL_DERIVEDNODE(FMCIONode)

public:
  explicit FMCIONode(const uhal::Node& node);
  virtual ~FMCIONode();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief      Reset timing node.
   */
  void reset(const std::string& clock_config_file = "") const override;

  /**
   * @brief      Fill hardware monitoring structure.
   */
  void get_info(timinghardwareinfo::TimingFMCMonitorData& mon_data) const;

  /**
   * @brief      Fill hardware monitoring structure.
   */
  void get_info(timinghardwareinfo::TimingFMCMonitorDataDebug& mon_data) const;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_FMCIONODE_HPP_