/**
 * @file MIBV3IONode.hpp
 *
 * MIBV3IONode is a class providing an interface
 * to the MIB V3 IO firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_MIBV3IONODE_HPP_
#define TIMING_INCLUDE_TIMING_MIBV3IONODE_HPP_

// PDT Headers
#include "timing/MIBV2IONode.hpp"

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
class MIBV3IONode : public MIBV2IONode
{
  UHAL_DERIVEDNODE(MIBV3IONode)

public:
  explicit MIBV3IONode(const uhal::Node& node);
  virtual ~MIBV3IONode();
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_MIBV3IONODE_HPP_
