/**
 * @file GIBV3IONode.hpp
 *
 * GIBV3IONode is a class providing an interface
 * to the GIB V3 IO firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_GIBV3IONODE_HPP_
#define TIMING_INCLUDE_TIMING_GIBV3IONODE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/GIBV2IONode.hpp"

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
class GIBV3IONode : public GIBV2IONode
{
  UHAL_DERIVEDNODE(GIBV3IONode)

public:
  explicit GIBV3IONode(const uhal::Node& node);
  virtual ~GIBV3IONode();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief      Read the contents of the IO expanders.
   */
  uint32_t read_io_expanders() const override; // NOLINT(build/unsigned)

  /**
   * @brief      Retrive SFP LOS status for all SFPs.
   */
  uint8_t read_sfps_los() const override; // NOLINT(build/unsigned)

  /**
   * @brief      Retrive SFP fault status for all SFPs.
   */
  uint8_t read_sfps_fault() const override; // NOLINT(build/unsigned)
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_GIBV3IONODE_HPP_
