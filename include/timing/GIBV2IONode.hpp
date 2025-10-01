/**
 * @file GIBV2IONode.hpp
 *
 * GIBV2IONode is a class providing an interface
 * to the MIB V2 IO firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_GIBV2IONODE_HPP_
#define TIMING_INCLUDE_TIMING_GIBV2IONODE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/GIBIONode.hpp"

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
class GIBV2IONode : public GIBIONode
{
  UHAL_DERIVEDNODE(GIBV2IONode)

public:
  explicit GIBV2IONode(const uhal::Node& node);
  virtual ~GIBV2IONode();

  /**
   * @brief     Clocks ready?
   */
  bool clocks_ok() const override;

private:
  void validate_sfp_id(uint32_t sfp_id) const override; // NOLINT(build/unsigned)
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_GIBV2IONODE_HPP_
